
#pragma once

#include <lookup.hpp>
#include <paths.hpp>

#include <fuse.h>

#include <utility>

namespace handlers {

namespace __detail {

constexpr auto fillDirectory = [](const auto& path) {
    return [&](auto, auto buf, auto filler, auto, auto) {
        return [=, &path](auto&... deps) {

            auto entries = lookup::directoryEntries(path)
                (std::forward<decltype(deps)>(deps)...);

            filler(buf, ".", nullptr, 0);
            filler(buf, "..", nullptr, 0);
            for (const auto& s : entries) {
                filler(buf, s.c_str(), nullptr, 0);
            }

            return 0;
        };
    };
};

} // namespace __detail

auto readdir(const paths::DatabaseQueryPath& path) {
    return __detail::fillDirectory(path);
}

auto readdir(const paths::DevicePath& path) {
    return __detail::fillDirectory(path);
}

auto readdir(const paths::DeviceAttributesPath& path) {
    return __detail::fillDirectory(path);
}

auto readdir(const paths::AttributePath& path) {
    return __detail::fillDirectory(path);
}

template <typename Path>
auto readdir(const Path&) {
    return [](auto...) {
        return [](auto&&...) {
            return -EIO;
        };
    };
}

} // namespace handlers
