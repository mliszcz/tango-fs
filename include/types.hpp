
#pragma once

#include <algorithm>
#include <type_traits>
#include <boost/optional.hpp>

// TODO: change boost::optional to std::optional in C++17

template <typename T>
using Maybe = boost::optional<T>;

constexpr auto& None = boost::none;

template <typename T>
constexpr inline auto Just(const T& t) {
    return boost::make_optional(t);
}

template <typename A, typename F>
inline auto operator>=(Maybe<A> s, F f) -> Maybe<decltype(f(*s))> {
    if (s) {
        try {
            return f(*s);
        } catch (...) {
            // left blank
        }
    }
    return None;
}

template <template <typename ...U> typename C, typename A, typename F>
inline auto operator>=(C<A> s, F f) {
    C<decltype(f(*s.begin()))> c{};
    std::transform(s.begin(), s.end(), std::inserter(c, c.begin()), f);
    return c;
}

// template <template <typename ...U> typename C, typename A, typename B>
// inline auto operator>=(C<A> s, std::function<B(A)> f) {
//     C<decltype(f(*s.begin()))> c{};
//     std::transform(s.begin(), s.end(), std::inserter(c, c.begin()), f);
//     return c;
// }
