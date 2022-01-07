//  ----------------------------------------------------------------------------
//
//  Copyright (c) 2019 - 2022 by the OpFlow developers
//
//  This file is part of OpFlow.
//
//  OpFlow is free software and is distributed under the MPL v2.0 license.
//  The full text of the license can be found in the file LICENSE at the top
//  level directory of OpFlow.
//
//  ----------------------------------------------------------------------------

#ifndef OPFLOW_CSRMATRIXGENERATOR_HPP
#define OPFLOW_CSRMATRIXGENERATOR_HPP

#include "Core/Equation/StencilHolder.hpp"
#include "Core/Meta.hpp"
#include "DataStructures/Matrix/CSRMatrix.hpp"
#include <vector>

namespace OpFlow {
    struct CSRMatrixGenerator {
        template <typename S, typename M>
        static auto generate(S& s, M&& mapper, const std::vector<bool>& pin_flags) {
            DS::CSRMatrix csr;

            Meta::static_for<S::size>([&]<int i>(Meta::int_<i>) {
                DS::CSRMatrix m = generate<i + 1>(s, mapper, pin_flags[i]);
                csr.append(m);
            });

            return csr;
        }

        template <std::size_t iTarget, typename S>
        static auto generate(S& s, auto&& mapper, bool pinValue) {
            DS::CSRMatrix mat;
            auto target = s.template getTarget<iTarget>();
            auto commStencil = s.comm_stencils[iTarget - 1];
            auto& uniEqn = s.template getEqnExpr<iTarget>();
            auto local_range = DS::commonRange(target->assignableRange, target->localRange);
            // prepare: evaluate the common stencil & pre-fill the arrays
            int stencil_size = commStencil.pad.size();

            struct m_tuple {
                int r, c;
                Real v;
                bool operator<(const m_tuple& other) const {
                    return r < other.r || (r == other.r && c < other.c);
                }
            };
            DS::DenseVector<m_tuple> coo;
            coo.resize(local_range.count() * stencil_size);
            mat.resize(local_range.count(), stencil_size);
            rangeFor(local_range, [&](auto&& i) {
                auto r = mapper(i, iTarget);// r is the local rank
                auto currentStencil = uniEqn.evalAt(i);
                int count = 0;
                if (pinValue && r == 0) {
                    coo[r * stencil_size] = m_tuple(
                            0,
                            mapper(DS::ColoredIndex<typename decltype(local_range)::base_index_type> {
                                    i, iTarget}),
                            1);
                    mat.rhs[r] = 0.;
                    count++;
                } else {
                    for (const auto& [key, v] : currentStencil.pad) {
                        auto idx = mapper(key);
                        coo[r * stencil_size + count++] = m_tuple(r, idx, v);
                    }
                    mat.rhs[r] = -currentStencil.bias;
                }
                for (; count < stencil_size; ++count)
                    coo[r * stencil_size + count]
                            = m_tuple(std::numeric_limits<int>::max(), std::numeric_limits<int>::max(), 0.);
            });
            oneapi::tbb::parallel_sort(coo.begin(), coo.end());
            auto iter = std::lower_bound(
                    coo.begin(), coo.end(),
                    m_tuple(std::numeric_limits<int>::max(), std::numeric_limits<int>::max(), 0.));
            coo.resize(iter - coo.begin());
            std::vector<std::atomic_int> nnz_counts(local_range.count());
            int common_base = coo.front().r;
            oneapi::tbb::parallel_for_each(coo.begin(), coo.end(),
                                           [&](const m_tuple& t) { nnz_counts[t.r - common_base]++; });
            DS::DenseVector<int> nnz_prefix(local_range.count() + 1); nnz_prefix[0] = 0;
            oneapi::tbb::parallel_scan(
                    oneapi::tbb::blocked_range<int>(0, nnz_counts.size()), 0,
                    [&](const oneapi::tbb::blocked_range<int>& r, int sum, bool is_final) {
                        int temp = sum;
                        for (int i = r.begin(); i < r.end(); ++i) {
                            temp += nnz_counts[i];
                            if (is_final) nnz_prefix[i + 1] = temp;
                        }
                        return temp;
                    },
                    [](int l, int r) { return l + r; });
            // copy to the global array
            oneapi::tbb::parallel_for(0, local_range.count(), [&](int i) { mat.row[i] = nnz_prefix[i]; });
            mat.row.back() = coo.size();
            oneapi::tbb::parallel_for(0, (int) coo.size(), [&](int i) { mat.col[i] = coo[i].c; });
            oneapi::tbb::parallel_for(0, (int) coo.size(), [&](int i) { mat.val[i] = coo[i].v; });
            mat.trim(coo.size());

            return mat;
        }
    };
}// namespace OpFlow

#endif//OPFLOW_CSRMATRIXGENERATOR_HPP
