#pragma once

#include <FuseOperations.hpp>
#include <TangoBasePath.hpp>

#include <algorithm>
#include <string>
#include <memory>

#include <cstring>
#include <tango.h>
#include <errno.h>
#include <fuse.h>

// http://fuse.sourceforge.net/doxygen/structfuse__operations.html
// http://sourceforge.net/p/fuse/wiki/Main_Page/
// http://sourceforge.net/p/fuse/wiki/FuseInvariants/

class TangoFuseOperations : public FuseOperations {

private:
    const char* hello_str;
    const char* hello_path;

public:

    TangoFuseOperations() :
        hello_str("Hello World!\n"),
        hello_path("/hello") { }

    int getattr(const char* path,
                struct stat* stbuf) override {

        auto p = mapPath(path);
        auto entry = p->getattr();

        if (entry->isValid()) {
            *stbuf = static_cast<struct stat>(*entry);
            return 0;
        } else {
            return -ENOENT;
        }
    }

    int readdir(const char* path,
                void* buf,
                fuse_fill_dir_t filler,
                off_t offset,
                struct fuse_file_info* fi) override {

        auto p = mapPath(path);

        filler(buf, ".", nullptr, 0);
        filler(buf, "..", nullptr, 0);

        for (const auto& s : p->readdir()) {
            filler(buf, s.c_str(), nullptr, 0);
        }

        return 0;
    }

    int open(const char* path,
             struct fuse_file_info* fi) override {

        auto p = mapPath(path);

        if (not p->getattr()->isValid()) {
            return -ENOENT;
        }

        return p->open(*fi);

        // if (strcmp(path, hello_path) != 0)
        //         return -ENOENT;
        // if ((fi->flags & 3) != O_RDONLY)
        //         return -EACCES;
        // return 0;
    }

    int read(const char* path,
             char* buf,
             size_t size,
             off_t offset,
             struct fuse_file_info* fi) override {

        auto p = mapPath(path);

        if (not p->getattr()->isValid()) {
         return -ENOENT;
        }

        auto data = p->read();
        data.push_back('\n');

        int bytesRead = 0;
        auto dataSize = data.size();

        if (offset < dataSize) {
            size = (offset + size > dataSize) ? (dataSize - offset) : size;
            std:copy(data.begin() + offset, data.begin() + offset + size, buf);
            bytesRead = size;
        }

        return bytesRead;
    }

private:

    const char SEPARATOR = '/';
    const std::string ROOT_PATH = std::string(1, SEPARATOR);

    enum class DevicePath {
        CLASS, NAME, DESCRIPTION, STATUS, UNDEFINED
    };

    DevicePath encodeDevicePath(const std::string& str) {
        if (str == "class") return DevicePath::CLASS;
        if (str == "status") return DevicePath::STATUS;
        if (str == "name") return DevicePath::NAME;
        if (str == "description") return DevicePath::DESCRIPTION;
        return DevicePath::UNDEFINED;
    }

    TangoBasePath::Ptr mapPath(const std::string& path) {

        if (path == ROOT_PATH) {
            return std::make_shared<TangoRootPath>(ROOT_PATH);
        }

        auto count = std::count(path.begin(), path.end(), SEPARATOR);
        switch (count) {
            case 1: return std::make_shared<TangoDomainPath>(path);
            case 2: return std::make_shared<TangoFamilyPath>(path);
            case 3: return std::make_shared<TangoMemberPath>(path);
            case 4: {
                auto pos = path.find_last_of(SEPARATOR);
                auto device = path.substr(0, pos);
                auto what = path.substr(pos+1);
                switch(encodeDevicePath(what)) {
                    case DevicePath::CLASS:
                        return std::make_shared<TangoDeviceInfoPath>(device,
                                            &Tango::DeviceInfo::dev_class);
                    case DevicePath::NAME:
                        return std::make_shared<TangoDeviceProxyPath>(device,
                                            &Tango::DeviceProxy::name);
                    case DevicePath::STATUS:
                        return std::make_shared<TangoDeviceProxyPath>(device,
                                            &Tango::DeviceProxy::status);
                    case DevicePath::DESCRIPTION:
                        return std::make_shared<TangoDeviceProxyPath>(device,
                                            &Tango::DeviceProxy::description);
                    default:
                        return std::make_shared<TangoInvalidPath>(path);
                }
            }
            default: return std::make_shared<TangoInvalidPath>(path);
        }
    }

};
