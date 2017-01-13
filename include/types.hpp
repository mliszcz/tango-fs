
#pragma once

#include <tango.h>

#include <algorithm>
#include <boost/optional.hpp>
#include <iostream>
#include <stdexcept>

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
        } catch (const std::exception& e) {
            std::clog << "[ERROR] exception: " << e.what() << "\n";
        } catch (const Tango::DevFailed& e) {
            std::clog << "[ERROR] Tango exception: " << e.errors[0].desc << "\n";
        } catch (...) {
            std::clog << "[ERROR] unknown exception\n";
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
