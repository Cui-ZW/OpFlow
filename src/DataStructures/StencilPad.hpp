#ifndef OPFLOW_STENCILPAD_HPP
#define OPFLOW_STENCILPAD_HPP

#include "Core/BasicDataTypes.hpp"
#include "Core/Meta.hpp"
#include <algorithm>
#include <unordered_map>

namespace OpFlow::DS {
    template <typename K, typename V>
    struct fake_map {
    private:
        std::array<std::pair<K, V>, 21> val;

        int _size = 0;

    public:
        fake_map() = default;

        auto& operator[](const K& key) {
            auto pos = -1;
            for (auto i = 0; i < _size; ++i) {
                if (key == val[i].first) {
                    pos = i;
                    break;
                }
            }
            if (pos == -1) {
                val[_size].first = key;
                _size++;
                return val[_size - 1].second;
            } else {
                return val[pos].second;
            }
        }

        auto size() const { return _size; }

        auto find(const K& k) {
            for (auto iter = val.begin(); iter != end(); ++iter) {
                if (iter->first == k) return iter;
            }
            return end();
        }

        auto find(const K& k) const {
            for (auto iter = val.begin(); iter != end(); ++iter) {
                if (iter->first == k) return iter;
            }
            return end();
        }

        auto findFirst(auto&& f) const {
            for (auto iter = val.begin(); iter != end(); ++iter) {
                if (f(iter->first)) return iter;
            }
            return end();
        }

        auto exist(const K& k) const {
            auto p = find(k);
            return p == end();
        }

        int rank(const K& k) const {
            auto p = find(k);
            if (p == end()) return -1;
            else
                return p - begin();
        }

        auto begin() { return val.begin(); }

        auto begin() const { return val.begin(); }

        auto end() { return val.begin() + _size; }

        auto end() const { return val.begin() + _size; }

        auto sort() {
            std::sort(val.begin(), val.end(), [](auto&& a, auto&& b) { return a.first < b.first; });
        }
    } __attribute__((aligned(32)));

    template <typename Idx>
    struct StencilPad {
        //todo: fix 1 size error
        fake_map<Idx, Real> pad {};
        Real bias = 0.;

        StencilPad() = default;
        StencilPad(Real b) : bias(b) {}

        auto toString(int level = 1) const {
            std::string prefix;
            for (auto i = 0; i < level; ++i) prefix += "\t";
            std::string ret = "\n" + prefix + "pad:\n";
            for (const auto& [k, v] : pad) {
                ret += prefix + "\t" + fmt::format("({})\t {}\n", k.toString(), v);
            }
            ret += prefix + fmt::format("bias: {}", bias);
            return ret;
        }

        auto& operator+=(const StencilPad& other) {
            for (const auto& [idx, val] : other.pad) {
                if (auto iter = pad.find(idx); iter != pad.end()) {
                    iter->second += val;
                } else {
                    pad[idx] = val;
                }
            }
            bias += other.bias;
            return *this;
        }

        auto& operator-=(const StencilPad& other) {
            for (const auto& [idx, val] : other.pad) {
                if (auto iter = pad.find(idx); iter != pad.end()) {
                    iter->second -= val;
                } else {
                    pad[idx] = -val;
                }
            }
            bias -= other.bias;
            return *this;
        }

        auto& operator*=(Real r) {
            for (auto& [idx, val] : pad) { val *= r; }
            bias *= r;
            return *this;
        }

        auto& operator/=(Real r) {
            for (auto& [idx, val] : pad) { val /= r; }
            bias /= r;
            return *this;
        }

        void sort() { pad.sort(); }
    };

    template <typename Idx>
    auto operator+(const StencilPad<Idx>& a, const StencilPad<Idx>& b) {
        auto ret = a;
        ret += b;
        return ret;
    }

    template <typename Idx, Meta::Numerical Num>
    auto operator+(const StencilPad<Idx>& a, Num b) {
        auto ret = a;
        ret += b;
        return ret;
    }

    template <typename Idx, Meta::Numerical Num>
    auto operator+(Num a, const StencilPad<Idx>& b) {
        auto ret = b;
        ret += a;
        return ret;
    }

    template <typename Idx>
    auto operator-(const StencilPad<Idx>& a, const StencilPad<Idx>& b) {
        auto ret = b * -1.0;
        return a + ret;
    }

    template <typename Idx, Meta::Numerical Num>
    auto operator-(const StencilPad<Idx>& a, Num b) {
        return a + (-b);
    }

    template <typename Idx, Meta::Numerical Num>
    auto operator-(Num a, const StencilPad<Idx>& b) {
        return StencilPad<Idx>(a) - b;
    }

    template <typename Idx, Meta::Numerical Num>
    auto operator*(const StencilPad<Idx>& a, Num b) {
        auto ret = a;
        for (auto& [idx, val] : ret.pad) { val *= b; }
        ret.bias *= b;
        return ret;
    }

    template <typename Idx, Meta::Numerical Num>
    auto operator*(Num b, const StencilPad<Idx>& a) {
        return a * b;
    }

    template <typename Idx, Meta::Numerical Num>
    auto operator/(const StencilPad<Idx>& a, Num b) {
        return a * (1. / b);
    }

    template <typename Idx>
    auto getOffsetStencil(const StencilPad<Idx>& a, const Idx& base) {
        using Offset = decltype(base - base);
        StencilPad<Offset> ret;
        for (const auto& [idx, val] : a.pad) { ret.pad[idx - base] = val; }
        ret.bias = a.bias;
        return ret;
    }

    template <typename Idx>
    auto commonStencil(const StencilPad<Idx>& a, const Idx& base,
                       const StencilPad<decltype(base - base)>& offsetStencil) {
        auto a_offset = getOffsetStencil(a, base);
        auto ret = a_offset;
        for (const auto& [idx, val] : offsetStencil.pad) {
            if (auto iter = ret.pad.find(idx); iter == ret.pad.end()) { ret.pad[idx] = 0.; }
        }
        return ret;
    }

    /// Return the common stencil based on a base.
    /// \tparam Idx
    /// \param a
    /// \param base
    /// \return
    template <typename Idx>
    auto commonStencil(const StencilPad<Idx>& a, const StencilPad<Idx>& base) {
        auto ret = a;
        for (const auto& [idx, val] : base.pad) {
            if (auto iter = ret.pad.find(idx); iter == ret.pad.end()) { ret.pad[idx] = 0.; }
        }
        return ret;
    }

    /// Get the merged stencil based on key. Value is not considered.
    /// \tparam Idx
    /// \param a
    /// \param b
    /// \return
    template <typename Idx>
    auto mergeStencil(const StencilPad<Idx>& a, const StencilPad<Idx>& b) {
        auto ret = a;
        for (const auto& [idx, val] : b.pad) {
            if (auto iter = ret.pad.find(idx); iter == ret.pad.end()) { ret.pad[idx] = val; }
        }
        return ret;
    }
}// namespace OpFlow::DS
#endif//OPFLOW_STENCILPAD_HPP
