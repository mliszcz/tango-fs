
#pragma once

#include <paths.hpp>
#include <tango.hpp>

#include <fuse.h>
#include <boost/variant.hpp>

namespace handlers {


class read : boost::static_visitor<int> {

private:

    char* buf;
    size_t size;
    off_t offset;
    struct fuse_file_info* fi;

    int readFromString(std::string data) const {

        int bytesRead = 0;
        auto dataSize = data.size();

        if (offset < (off_t)dataSize) {
            bytesRead = (offset + size > dataSize) ? (dataSize - offset) : size;
            std::copy_n(data.begin() + offset, bytesRead, buf);
        }

        return bytesRead;
    }

    template <typename Path>
    int readData(const Path& path) const {
        auto f = [&](const auto& s){ return this->readFromString(s); };
        return (lookup::fileContents(path) >= f).get_value_or(-EIO);
    }

public:

    read(char* buf, size_t size, off_t offset, struct fuse_file_info* fi)
        : buf(buf)
        , size(size)
        , offset(offset)
        , fi(fi) {}

    int operator()(const paths::InvalidPath&) const {
        return -ENOENT;
    }

    int operator()(const paths::DeviceClassPath& path) const {
        return readData(path);
    }

    int operator()(const paths::DeviceDescriptionPath& path) const {
        return readData(path);
    }

    int operator()(const paths::DeviceNamePath& path) const {
        return readData(path);
    }

    int operator()(const paths::DeviceStatusPath& path) const {
        return readData(path);
    }

    int operator()(const paths::AttributeValuePath& path) const {
        return readData(path);
    }

    template <typename Path>
    int operator()(const Path&) const {
        return -EIO;
    }
};

}
