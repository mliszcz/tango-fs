
#pragma once

#include <paths.hpp>
#include <lookup.hpp>

#include <fuse.h>
#include <boost/variant.hpp>

namespace handlers {

using Stat = struct stat;

template <typename Path>
auto getattr(const Path& path) {
    return [=](auto&& stbuf) {
    // return [=](struct stat* stbuf) {
        return [=](auto&& db_, auto&& dp_) {

        auto db = tango::createDatabase;
        auto dp = tango::createDeviceProxy;

        auto size = lookup::fileContents(path)(db, dp)
            >= std::mem_fn(&std::string::size);

        Stat stat {};
        stat.st_mode = S_IFREG | 0444;
        stat.st_nlink = 1;
        stat.st_size = size.get_value_or(0);
        *stbuf = stat;
        return 0;
        };
    };
}


template <typename Path>
auto fillAsDirectory(const Path& path) {
    // return [=](struct stat* stbuf) {
    return [=](auto&& stbuf) {
        return [=](auto&& db_, auto&& dp_) {

        auto entries = lookup::directoryEntries(path)(tango::createDatabase,
                                                      tango::createDeviceProxy);
        Stat stat {};
        stat.st_mode = S_IFDIR | 0755;
        stat.st_nlink = 2 + entries.size();
        *stbuf = stat;

        return 0;
        };
    };
}

auto getattr(const paths::InvalidPath&) {
// return [=](struct stat* stbuf) {
    return [=](auto&& stbuf) {
        return [=](auto&& db_, auto&& dp_) {

        return -ENOENT;
        };
    };
}

auto getattr(const paths::DatabaseQueryPath& path) {
    return fillAsDirectory(path);
}

auto getattr(const paths::DevicePath& path) {
    return fillAsDirectory(path);
}

auto getattr(const paths::DeviceAttributesPath& path) {
    return fillAsDirectory(path);
}

auto getattr(const paths::AttributePath& path) {
    return fillAsDirectory(path);
}

}
