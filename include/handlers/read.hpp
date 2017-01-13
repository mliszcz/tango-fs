
#pragma once

#include <lookup.hpp>
#include <paths.hpp>
#include <types.hpp>

#include <fuse.h>

#include <algorithm>
#include <utility>

namespace handlers {

namespace __detail {

constexpr auto readData = [](const auto& path) {
    return [&](auto, auto buf, auto size, auto offset, auto fi) {
        return [=, &path](auto&&... deps) {

            auto readFromString = [&](const auto& data) {

                auto bytesRead = 0;
                auto dataSize = data.size();

                if (offset < (decltype(offset))dataSize) {
                    bytesRead = std::min<int>(dataSize - offset, size);
                    std::copy_n(data.begin() + offset, bytesRead, buf);
                }

                return bytesRead;
            };

            auto size = lookup::fileContents(path)
                (std::forward<decltype(deps)>(deps)...)
                >= readFromString;

            return size.value_or(-EIO);
        };
    };
};

} // namespace __detail

auto read(const paths::DeviceClassPath& path) {
    return __detail::readData(path);
}

auto read(const paths::DeviceDescriptionPath& path) {
    return __detail::readData(path);
}

auto read(const paths::DeviceNamePath& path) {
    return __detail::readData(path);
}

auto read(const paths::DeviceStatusPath& path) {
    return __detail::readData(path);
}

auto read(const paths::AttributeValuePath& path) {
    return __detail::readData(path);
}

template <typename Path>
auto read(const Path&) {
    return [](auto...) {
        return [](auto&&...) {
            return -EIO;
        };
    };
}

} // namespace handlers
