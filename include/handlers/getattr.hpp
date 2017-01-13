
#pragma once

#include <lookup.hpp>
#include <paths.hpp>
#include <types.hpp>

#include <fuse.h>

#include <type_traits>
#include <utility>

namespace handlers {

namespace __detail {

constexpr auto fillAsFile = [](const auto& path) {
    return [&](auto, auto stbuf) {
        return [=, &path](auto&&... deps) {

            auto size = lookup::fileContents(path)
                (std::forward<decltype(deps)>(deps)...)
                >= std::mem_fn(&std::string::size);

            std::remove_reference_t<decltype(*stbuf)> stat {};
            stat.st_mode = S_IFREG | 0444;
            stat.st_nlink = 1;
            stat.st_size = size.get_value_or(0);
            *stbuf = stat;

            return 0;
        };
    };
};

constexpr auto fillAsDirectory = [](const auto& path) {
    return [&](auto, auto stbuf) {
        return [=, &path](auto&&... deps) {

            auto entries = lookup::directoryEntries(path)
                (std::forward<decltype(deps)>(deps)...);

            std::remove_reference_t<decltype(*stbuf)> stat {};
            stat.st_mode = S_IFDIR | 0755;
            stat.st_nlink = 2 + entries.size();
            *stbuf = stat;

            return 0;
        };
    };
};

} // namespace __detail

template <typename Path>
auto getattr(const Path& path) {
    return __detail::fillAsFile(path);
}

auto getattr(const paths::DatabaseQueryPath& path) {
    return __detail::fillAsDirectory(path);
}

auto getattr(const paths::DevicePath& path) {
    return __detail::fillAsDirectory(path);
}

auto getattr(const paths::DeviceAttributesPath& path) {
    return __detail::fillAsDirectory(path);
}

auto getattr(const paths::AttributePath& path) {
    return __detail::fillAsDirectory(path);
}

auto getattr(const paths::InvalidPath&) {
    return [](auto...) {
        return [](auto&&...) {
            return -ENOENT;
        };
    };
}

} // namespace handlers
