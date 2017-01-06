
#pragma once

#include <paths.hpp>
#include <tango.hpp>
#include <types.hpp>

#include <lookup.hpp>

#include <fuse.h>
#include <tango.h>

#include <boost/variant.hpp>

namespace handlers {

class readdir : boost::static_visitor<int> {

private:

    void* buf;
    fuse_fill_dir_t filler;
    off_t offset;
    struct fuse_file_info* fi;

    template<typename Path>
    void fillDirectory(const Path& path) const {
        auto entries = lookup::directoryEntries(path);
        filler(buf, ".", nullptr, 0);
        filler(buf, "..", nullptr, 0);
        for (const auto& s : entries) {
            filler(buf, s.c_str(), nullptr, 0);
        }
    }

public:

    readdir(void* buf,
            fuse_fill_dir_t filler,
            off_t offset,
            struct fuse_file_info* fi)
        : buf(buf)
        , filler(filler)
        , offset(offset)
        , fi(fi) {}

    int operator()(const paths::DatabaseQueryPath& path) const {
        fillDirectory(path);
        return 0;
    }

    int operator()(const paths::DeviceAttributesPath& path) const {
        fillDirectory(path);
        return 0;
    }

    int operator()(const paths::DevicePath& path) const {
        fillDirectory(path);
        return 0;
    }

    template <typename Path>
    int operator()(const Path&) const {
        return -ENOENT;
    }
};

}
