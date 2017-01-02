
#pragma once

#include <paths.hpp>
#include <fuse.h>
#include <boost/variant.hpp>

namespace handlers {

class open : boost::static_visitor<int> {

private:

    struct fuse_file_info* fi;

public:

    open(struct fuse_file_info* fi)
        : fi(fi) {}

    int operator()(const paths::InvalidPath&) const {
        return -ENOENT;
    }

    template <typename Path>
    int operator()(const Path&) const {
        return 0;
    }
};

}
