#ifndef OPFLOW_STRUCTSOLVERJACOBI_HPP
#define OPFLOW_STRUCTSOLVERJACOBI_HPP

#include "StructSolver.hpp"

namespace OpFlow {
    template <>
    struct StructSolverParams<StructSolverType::Jacobi> : StructSolverParamsBase {
        std::optional<bool> useZeroGuess;
    };

    template <>
    struct StructSolver<StructSolverType::Jacobi> {
        using Param = StructSolverParams<StructSolverType::Jacobi>;
        constexpr auto static type = StructSolverType::Jacobi;
        Param params;

        StructSolver() { HYPRE_StructJacobiCreate(params.comm, &solver); }
        explicit StructSolver(const Param& p) : params(p) { HYPRE_StructJacobiCreate(params.comm, &solver); }
        ~StructSolver() { HYPRE_StructJacobiDestroy(solver); }
        StructSolver(const StructSolver& s) : params(s.params) {
            HYPRE_StructJacobiCreate(params.comm, &solver);
        }
        StructSolver(StructSolver&& s) noexcept : params(std::move(s.params)), solver(std::move(s.solver)) {
            s.solver = nullptr;
        }

        void init() {
            if (params.tol) HYPRE_StructJacobiSetTol(solver, params.tol.value());
            if (params.maxIter) HYPRE_StructJacobiSetMaxIter(solver, params.maxIter.value());
            if (params.useZeroGuess && params.useZeroGuess.value_or(false))
                HYPRE_StructJacobiSetZeroGuess(solver);
            if (params.useZeroGuess && !params.useZeroGuess.value_or(true))
                HYPRE_StructJacobiSetNonZeroGuess(solver);
        }

        auto solve(HYPRE_StructMatrix& A, HYPRE_StructVector& b, HYPRE_StructVector& x) {
            return HYPRE_StructJacobiSolve(solver, A, b, x);
        }

        auto setup(HYPRE_StructMatrix& A, HYPRE_StructVector& b, HYPRE_StructVector& x) {
            return HYPRE_StructJacobiSetup(solver, A, b, x);
        }

        auto& getSolver() { return solver; }
        const auto& getSolver() const { return solver; }
        auto getSolveFunc() const { return HYPRE_StructJacobiSolve; }
        auto getSetUpFunc() const { return HYPRE_StructJacobiSetup; }

        auto getIterNum() const {
            int ret;
            HYPRE_StructJacobiGetNumIterations(solver, &ret);
            return ret;
        }

        auto getFinalRes() const {
            Real ret;
            HYPRE_StructJacobiGetFinalRelativeResidualNorm(solver, &ret);
            return ret;
        }

    private:
        HYPRE_StructSolver solver;
    };

}// namespace OpFlow
#endif//OPFLOW_STRUCTSOLVERJACOBI_HPP
