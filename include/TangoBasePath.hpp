#pragma once

#include <FsEntry.hpp>

#include <string>
#include <vector>
#include <set>
#include <memory>
#include <iterator>
#include <algorithm>
#include <functional>
#include <fuse.h>
#include <tango.h>

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

struct TangoDbQueryPath : public TangoBaseDirPath {
    using TangoBaseDirPath::TangoBaseDirPath;

    std::set<std::string> readdir() override {
        return findChildren();
    }

private:
    std::vector<std::string> queryDatabase() {
        std::string queryPath = path.substr(1) + "*";
        auto database = std::make_shared<Tango::Database>();
        std::vector<std::string> result;
        Tango::DbDatum dd = database->get_device_exported(queryPath);
        dd >> result;
        return result;
    }

    std::set<std::string> findChildren() {
        const auto& db = queryDatabase();
        std::set<std::string> res;
        const auto& normalizedPath = (path == "/") ? "" : path;
        std::transform(db.begin(), db.end(), std::inserter(res, res.begin()),
        [&](const auto& x){
            const auto& s = (x).substr(normalizedPath.size());
            auto it = std::find(s.begin(), s.end(), '/');
            return std::string(s.begin(), it);
        });
        return res;
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

struct TangoDeviceFilePathWithProxy : public TangoBaseFilePath {
    using TangoBaseFilePath::TangoBaseFilePath;
protected:
    std::unique_ptr<Tango::DeviceProxy> getDeviceProxy() {
        try {
            auto tangoPath = path.substr(1);
            return std::make_unique<Tango::DeviceProxy>(tangoPath);
        } catch(...) {
            return nullptr;
        }
    }

    // FIXME const& not accepted here (mem_fn issue)
    std::string extractFromDeviceProxy(
        std::function<std::string(Tango::DeviceProxy)> f) {
        auto p = getDeviceProxy();
        if (p) {
            return f(*p);
        } else {
            return "";
        }
    }

    std::string extractFromDeviceInfo(
        std::function<std::string(Tango::DeviceInfo)> f) {
        return extractFromDeviceProxy([&](auto p){ return f(p.info()); });
    }

    std::string extractFromDeviceInfo(std::string Tango::DeviceInfo::* p) {
        return extractFromDeviceInfo([&](auto i){ return i.*p; });
    }
};

/**
 * Extracts data using DeviceInfo member.
 */
struct TangoDeviceInfoPath : public TangoDeviceFilePathWithProxy {

    TangoDeviceInfoPath(const std::string& path,
                        std::string Tango::DeviceInfo::* p) :
        TangoDeviceFilePathWithProxy(path),
        infoMember(p) { }

    std::string read() override {
        return extractFromDeviceInfo(infoMember);
    }

private:
    std::string Tango::DeviceInfo::* infoMember;
};

/**
 * Extracts data using DeviceProxy -> std::string function.
 */
struct TangoDeviceProxyPath : public TangoDeviceFilePathWithProxy {

    TangoDeviceProxyPath(const std::string& path,
                        std::string (Tango::DeviceProxy::*p)()) :
        TangoDeviceFilePathWithProxy(path),
        proxyFn(p) { }

    std::string read() override {
        return extractFromDeviceProxy(std::mem_fn(proxyFn));
    }

private:
    std::string (Tango::DeviceProxy::*proxyFn)();
};
