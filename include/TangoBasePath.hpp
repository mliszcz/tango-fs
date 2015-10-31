#pragma once

#include <FsEntry.hpp>

#include <string>
#include <vector>
#include <set>
#include <memory>
#include <iterator>
#include <algorithm>
#include <fuse.h>
#include <tango.h>

class TangoBasePath {
protected:
    std::string path;

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

// template <typename TFsEntry>
// struct TangoBaseFsPath : public TangoBasePath {
//     using TangoBasePath::TangoBasePath;
//
//     FsEntry::Ptr getattr() override {
//         return std::make_shared<TFsEntry>();
//     }
// };

struct TangoBaseFilePath : public TangoBasePath {
    using TangoBasePath::TangoBasePath;

    FsEntry::Ptr getattr() override {
        return std::make_shared<FsEntryFile>(read().size()+1);
    }

    int open(const struct fuse_file_info& fi) override {
        return 0;
    }
};

struct TangoDbQueryPath : public TangoBaseDirPath {
    using TangoBaseDirPath::TangoBaseDirPath;

    std::set<std::string> readdir() override {
        return findChildren();
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
            "name",
            "class",
            "server",
            "status",
            "attributes"
        };
    }
};

struct TangoDeviceClassPath : public TangoBaseFilePath {
    using TangoBaseFilePath::TangoBaseFilePath;

    std::string read() override {
        try {
            auto p = path.substr(1);
            Tango::DeviceProxy dp(p);
            auto info = dp.info();
            return info.dev_class;
        } catch (...) {
            return "";
        }
    }
};

struct TangoDeviceStatusPath : public TangoBaseFilePath {
    using TangoBaseFilePath::TangoBaseFilePath;

    std::string read() override {
        try {
            auto p = path.substr(1);
            Tango::DeviceProxy dp(p);
            return dp.status();
        } catch (...) {
            return "";
        }
    }
};
