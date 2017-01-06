
#pragma once

#include <paths.hpp>
#include <lookup.hpp>

#include <fuse.h>
#include <boost/variant.hpp>

namespace handlers {

class getattr : boost::static_visitor<int> {

private:

    using Stat = struct stat;
    Stat* stbuf;

    template <typename Path>
    void fillAsDirectory(const Path& path) const {
        Stat stat {};
        stat.st_mode = S_IFDIR | 0755;
        stat.st_nlink = 2 + lookup::directoryEntries(path).size();
        *stbuf = stat;
    }

public:

    getattr(Stat* stbuf)
        : stbuf(stbuf) {}

    int operator()(const paths::InvalidPath&) const {
        return -ENOENT;
    }

    int operator()(const paths::DatabaseQueryPath& path) const {
        fillAsDirectory(path);
        return 0;
    }

    int operator()(const paths::DevicePath& path) const {
        fillAsDirectory(path);
        return 0;
    }

    int operator()(const paths::DeviceAttributesPath& path) const {
        fillAsDirectory(path);
        return 0;
    }

    int operator()(const paths::AttributePath& path) const {
        fillAsDirectory(path);
        return 0;
    }

    template <typename Path>
    int operator()(const Path& path) const {

        auto size = lookup::fileContents(path)
            >= std::mem_fn(&std::string::size);

        Stat stat {};
        stat.st_mode = S_IFREG | 0444;
        stat.st_nlink = 1;
        stat.st_size = size.get_value_or(0);
        *stbuf = stat;
        return 0;
    }
};

}
