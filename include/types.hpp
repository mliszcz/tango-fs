
#pragma once

#include <algorithm>
#include <type_traits>
#include <boost/optional.hpp>

// TODO: change boost::optional to std::optional in C++17

// TODO: alias boost::none as None

template <typename T>
using Maybe = boost::optional<T>;

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
    return boost::none;
}

template <template <typename ...U> typename C, typename A, typename F>
inline auto operator>=(C<A> s, F f) {
    C<decltype(f(*s.begin()))> c{};
    std::transform(s.begin(), s.end(), std::inserter(c, c.begin()), f);
    return c;
}

template <typename A, typename H, typename... T>
struct is_any_same {
    static constexpr bool value =
        std::is_same<A, H>::value || is_any_same<A, T...>::value;
};

template <typename A, typename H>
struct is_any_same<A, H> {
    static constexpr bool value = std::is_same<A, H>::value;
};

// TODO: not working, fix
// template<typename A, typename H, typename... T>
// using enable_if_any_same_t =
//     typename std::enable_if_t<is_any_same<A, H, T...>::value>::type;
