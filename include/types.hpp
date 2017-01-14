
#pragma once

#include <tango.h>

#include <algorithm>
#include <iostream>
#include <stdexcept>

// TODO: remove if when gcc 7 is released

#if __has_include(<optional>)

#include <optional>
namespace stdx {
using namespace ::std;
}

#else

#include <experimental/optional>
namespace stdx {
using namespace ::std;
using namespace ::std::experimental;
}

#endif

template <typename T>
using Maybe = stdx::optional<T>;

template <typename T>
constexpr inline auto Just(const T& t) {
    return stdx::make_optional(t);
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
    return stdx::nullopt;
}

template <template <typename ...U> typename C, typename A, typename F>
inline auto operator>=(C<A> s, F f) {
    C<decltype(f(*s.begin()))> c{};
    std::transform(s.begin(), s.end(), std::inserter(c, c.begin()), f);
    return c;
}
