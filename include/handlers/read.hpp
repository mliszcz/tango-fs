
#pragma once

#include <paths.hpp>
#include <tango.hpp>
#include <fuse.h>
#include <boost/variant.hpp>

namespace handlers {

using DevProxyExtractor = std::function<std::string(Tango::DeviceProxy)>;

Maybe<std::string> extractFromDeviceProxy(
        const std::string& path,
        DevProxyExtractor f) {
    return tango::createDeviceProxy(path) >= f;
}

class read : boost::static_visitor<int> {

private:

    char* buf;
    size_t size;
    off_t offset;
    struct fuse_file_info* fi;

    int readFromString(std::string data) const {

        data.push_back('\n');

        int bytesRead = 0;
        auto dataSize = data.size();

        if (offset < dataSize) {
            bytesRead = (offset + size > dataSize) ? (dataSize - offset) : size;
            std:copy_n(data.begin() + offset, bytesRead, buf);
        }

        return bytesRead;
    }

    int readData(Maybe<std::string>&& data) const {
        return (data >= [&](const auto& s){ return this->readFromString(s); })
            .get_value_or(-EIO);
    }

    int readDeviceProxyField(const std::string& path,
                             DevProxyExtractor f) const {
        return readData(extractFromDeviceProxy(path, f));
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
        return readData(tango::createDeviceProxy(path.device)
            >= [](auto& proxy){ return proxy.info().dev_class; });
    }

    int operator()(const paths::DeviceDescriptionPath& path) const {
        return readDeviceProxyField(path.device, &Tango::DeviceProxy::description);
    }

    int operator()(const paths::DeviceNamePath& path) const {
        return readDeviceProxyField(path.device, &Tango::DeviceProxy::name);
    }

    int operator()(const paths::DeviceStatusPath& path) const {
        return readDeviceProxyField(path.device, &Tango::DeviceProxy::status);
    }


    template <typename Path>
    int operator()(const Path&) const {
        return -EIO;
    }
};

}
