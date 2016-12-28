#pragma once

#include <FsEntry.hpp>

#include <types.hpp>
#include <tango.hpp>

#include <string>
#include <vector>
#include <set>
#include <memory>
#include <iterator>
#include <algorithm>
#include <functional>
#include <fuse.h>
#include <tango.h>

#include <boost/optional.hpp>


class TangoBasePath {
protected:
    std::string path;

public:
    using Ptr = std::shared_ptr<TangoBasePath>;

    TangoBasePath(const std::string& path) :
        path(path) { }

    virtual FsEntry::Ptr getattr() = 0;

    virtual std::set<std::string> readdir() {
        return {};
    }

    virtual int open(const struct fuse_file_info& fi) {
        return -ENOENT;
    }

    virtual std::string read() {
        return {};
    }
};

struct TangoInvalidPath : public TangoBasePath {
    using TangoBasePath::TangoBasePath;

    FsEntry::Ptr getattr() override {
        return std::make_shared<FsEntryInvalid>();
    }
};

struct TangoBaseDirPath : public TangoBasePath {
    using TangoBasePath::TangoBasePath;

    FsEntry::Ptr getattr() override {
        return std::make_shared<FsEntryDirectory>();
    }
};

struct TangoBaseFilePath : public TangoBasePath {
    using TangoBasePath::TangoBasePath;

    FsEntry::Ptr getattr() override {
        return std::make_shared<FsEntryFile>(read().size()+1);
    }

    int open(const struct fuse_file_info& fi) override {
        // if ((fi.flags & 3) != O_RDONLY)
        //         return -EACCES;
        return 0;
    }
};

auto removeStringPrefix(std::string::size_type prefix) {
    return [=](const std::string& text) {
        return text.substr(prefix);
    };
}

auto removeStringAfter(std::string::value_type c) {
    return [=](const std::string& text) {
        return text.substr(0, text.find(c));
    };
}

auto findDirectChildrenInDatabase(const std::string& path) {

    auto childrenPath = path + "*";

    auto result = (tango::createDatabase()
        >= [&](auto& db){ return db.get_device_exported(childrenPath); }
        >= tango::extractFromDbDatum<std::vector<std::string>>)
        .get_value_or({});

    auto prefix = path.empty() ? 0 : path.size() + 1; // + 1 for leading slash

    auto entries = result
        >= removeStringPrefix(prefix)
        >= removeStringAfter('/');

    return std::set<std::string>(entries.begin(), entries.end());
}


struct TangoDbQueryPath : public TangoBaseDirPath {
    using TangoBaseDirPath::TangoBaseDirPath;

    std::set<std::string> readdir() override {
        return findDirectChildrenInDatabase(path.substr(1));
    }
};

struct TangoRootPath : public TangoDbQueryPath {
    using TangoDbQueryPath::TangoDbQueryPath;
};

struct TangoDomainPath : public TangoDbQueryPath {
    using TangoDbQueryPath::TangoDbQueryPath;
};

struct TangoFamilyPath : public TangoDbQueryPath {
    using TangoDbQueryPath::TangoDbQueryPath;
};

struct TangoMemberPath : public TangoBaseDirPath {
    using TangoBaseDirPath::TangoBaseDirPath;

    std::set<std::string> readdir() override {
        return {
            "attributes",
            "class",
            "description",
            "name",
            "status"
        };
    }
};

struct TangoDeviceInfoPath : public TangoBaseFilePath {

    TangoDeviceInfoPath(const std::string& path,
                        std::string Tango::DeviceInfo::* p) :
        TangoBaseFilePath(path),
        infoMember(p) { }

    std::string read() override {
        return (tango::createDeviceProxy(path.substr(1))
            >= [](auto& proxy){ return proxy.info(); }
            >= [&](auto& info){ return info.*infoMember; }).get_value_or("");
    }

private:
    std::string Tango::DeviceInfo::* infoMember;
};

struct TangoDeviceProxyPath : public TangoBaseFilePath {

    TangoDeviceProxyPath(const std::string& path,
                         std::function<std::string(Tango::DeviceProxy&)> p) :

        TangoBaseFilePath(path),
        proxyFn(p) { }

    std::string read() override {
        return (tango::createDeviceProxy(path.substr(1)) >= proxyFn).get_value_or("");
    }

private:
    std::function<std::string(Tango::DeviceProxy&)> proxyFn;
};
