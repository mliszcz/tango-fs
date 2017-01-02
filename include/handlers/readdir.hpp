
#pragma once

#include <paths.hpp>
#include <tango.hpp>
#include <types.hpp>

#include <fuse.h>
#include <tango.h>

#include <boost/variant.hpp>

#include <set>
#include <string>
#include <vector>

namespace handlers {

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

class readdir : boost::static_visitor<int> {

private:

    void* buf;
    fuse_fill_dir_t filler;
    off_t offset;
    struct fuse_file_info* fi;

    template<typename T>
    void fill(T&& paths) const {
        filler(buf, ".", nullptr, 0);
        filler(buf, "..", nullptr, 0);
        for (const auto& s : paths) {
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
        fill(findDirectChildrenInDatabase(path.query));
        return 0;
    }

    int operator()(const paths::DevicePath&) const {
        fill(std::set<std::string>{"attributes",
                                   "class",
                                   "description",
                                   "name",
                                   "status"});
        return 0;
    }

    template <typename Path>
    int operator()(const Path&) const {
        return -ENOENT;
    }
};

}
