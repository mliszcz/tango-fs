
#pragma once

#include <paths.hpp>
#include <fuse.h>
#include <boost/variant.hpp>

namespace handlers {

class getattr : boost::static_visitor<int> {

private:

    using Stat = struct stat;
    Stat* stbuf;

    void fillAsDirectory() const {
        Stat stat {};
        stat.st_mode = S_IFDIR | 0755;
        stat.st_nlink = 2;
        *stbuf = stat;
    }

public:

    getattr(Stat* stbuf)
        : stbuf(stbuf) {}

    int operator()(const paths::InvalidPath&) const {
        return -ENOENT;
    }

    int operator()(const paths::DatabaseQueryPath&) const {
        fillAsDirectory();
        return 0;
    }

    int operator()(const paths::DevicePath&) const {
        fillAsDirectory();
        return 0;
    }

    template <typename Path>
    int operator()(const Path&) const {
        Stat stat {};
        stat.st_mode = S_IFREG | 0444;
        stat.st_nlink = 1;
        stat.st_size = 0; // TODO: set size
        *stbuf = stat;
        return 0;
    }
};

}
