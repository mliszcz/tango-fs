
#pragma once

#include <paths.hpp>
#include <handlers/getattr.hpp>
#include <handlers/readdir.hpp>
#include <handlers/open.hpp>
#include <handlers/read.hpp>
#include <boost/variant.hpp>
#include <boost/format.hpp>
#include <regex>
#include <utility>

namespace filesystem {

constexpr auto makeFuseHandler = [](auto ...deps) {

    // TODO: forward lambda arguments
    // unfortunately, currently GCC does not allow for sth like
    // [args = std::forward<decltype(args)>(args)...](){}

    // TODO: explicit captures are required (probably) due to GCC bug
    // warning: 'args#0' is used uninitialized in this function

    return [deps...](auto&& handler) {
    return [handler, deps...](auto path, auto ...args) {

        std::string wordRegex = "[a-zA-Z0-9\\._-]+";

        auto re = [&](const std::string& s) {
            return std::regex(boost::str(boost::format(s) % wordRegex));
        };

        auto RE_ANY                = std::regex(".*");
        auto RE_ROOT               = std::regex("/?");
        auto RE_DOMAIN             = re("/(%1%)/?");
        auto RE_FAMILY             = re("/(%1%/%1%)/?");
        auto RE_DEVICE             = re("/(%1%/%1%/%1%)/?");
        auto RE_DEVICE_CLASS       = re("/(%1%/%1%/%1%)/class");
        auto RE_DEVICE_DESCRIPTION = re("/(%1%/%1%/%1%)/description");
        auto RE_DEVICE_NAME        = re("/(%1%/%1%/%1%)/name");
        auto RE_DEVICE_STATUS      = re("/(%1%/%1%/%1%)/status");
        auto RE_DEVICE_ATTRIBUTES  = re("/(%1%/%1%/%1%)/attributes/?");
        auto RE_ATTRIBUTE          = re("/(%1%/%1%/%1%)/attributes/(%1%)/?");
        auto RE_ATTRIBUTE_VALUE    = re("/(%1%/%1%/%1%)/attributes/(%1%)/value");

        std::string p{path};
        std::smatch m{};

        auto match = [&](auto&&... a) {
            return regex_match(p, m, std::forward<decltype(a)>(a)...);
        };

        auto f = [&](auto&& p) {
            return handler(std::forward<decltype(p)>(p))(args...)(deps...);
        };

        using namespace paths;

        if (match(RE_ROOT))               return f(DatabaseQueryPath{""});
        if (match(RE_DOMAIN))             return f(DatabaseQueryPath{m[1]});
        if (match(RE_FAMILY))             return f(DatabaseQueryPath{m[1]});
        if (match(RE_DEVICE))             return f(DevicePath{m[1]});
        if (match(RE_DEVICE_CLASS))       return f(DeviceClassPath{m[1]});
        if (match(RE_DEVICE_DESCRIPTION)) return f(DeviceDescriptionPath{m[1]});
        if (match(RE_DEVICE_NAME))        return f(DeviceNamePath{m[1]});
        if (match(RE_DEVICE_STATUS))      return f(DeviceStatusPath{m[1]});
        if (match(RE_DEVICE_ATTRIBUTES))  return f(DeviceAttributesPath{m[1]});
        if (match(RE_ATTRIBUTE))          return f(AttributePath{m[1], m[2]});
        if (match(RE_ATTRIBUTE_VALUE))    return f(AttributeValuePath{m[1], m[2]});

        return f(InvalidPath{});

    };
    };
};

// TODO remove
paths::AnyPath parsePath(const char* path) {

    using namespace paths;

    std::string id = "[a-zA-Z0-9\\._-]+";

    auto re = [&](const std::string& s) {
        return std::regex(boost::str(boost::format(s) % id));
    };

    auto RE_ANY                = std::regex(".*");
    auto RE_ROOT               = std::regex("/?");
    auto RE_DOMAIN             = re("/(%1%)/?");
    auto RE_FAMILY             = re("/(%1%/%1%)/?");
    auto RE_DEVICE             = re("/(%1%/%1%/%1%)/?");
    auto RE_DEVICE_CLASS       = re("/(%1%/%1%/%1%)/class");
    auto RE_DEVICE_DESCRIPTION = re("/(%1%/%1%/%1%)/description");
    auto RE_DEVICE_NAME        = re("/(%1%/%1%/%1%)/name");
    auto RE_DEVICE_STATUS      = re("/(%1%/%1%/%1%)/status");
    auto RE_DEVICE_ATTRIBUTES  = re("/(%1%/%1%/%1%)/attributes/?");
    auto RE_ATTRIBUTE          = re("/(%1%/%1%/%1%)/attributes/(%1%)/?");
    auto RE_ATTRIBUTE_VALUE    = re("/(%1%/%1%/%1%)/attributes/(%1%)/value");

    std::string p{path};
    std::smatch m{};

    auto match = [&](auto&&... args) {
        return regex_match(p, m, std::forward<decltype(args)>(args)...);
    };

    if (match(RE_ROOT))               return DatabaseQueryPath{""};
    if (match(RE_DOMAIN))             return DatabaseQueryPath{m[1]};
    if (match(RE_FAMILY))             return DatabaseQueryPath{m[1]};
    if (match(RE_DEVICE))             return DevicePath{m[1]};
    if (match(RE_DEVICE_CLASS))       return DeviceClassPath{m[1]};
    if (match(RE_DEVICE_DESCRIPTION)) return DeviceDescriptionPath{m[1]};
    if (match(RE_DEVICE_NAME))        return DeviceNamePath{m[1]};
    if (match(RE_DEVICE_STATUS))      return DeviceStatusPath{m[1]};
    if (match(RE_DEVICE_ATTRIBUTES))  return DeviceAttributesPath{m[1]};
    if (match(RE_ATTRIBUTE))          return AttributePath{m[1], m[2]};
    if (match(RE_ATTRIBUTE_VALUE))    return AttributeValuePath{m[1], m[2]};

    return InvalidPath{};
}

int readdir(const char* path,
            void* buf,
            fuse_fill_dir_t filler,
            off_t offset,
            struct fuse_file_info* fi) {
    auto p = parsePath(path);
    handlers::readdir handler{buf, filler, offset, fi};
    return boost::apply_visitor(handler, p);
}

int open(const char* path,
         struct fuse_file_info* fi) {
    auto p = parsePath(path);
    handlers::open handler{fi};
    return boost::apply_visitor(handler, p);
}

int read(const char* path,
         char* buf,
         size_t size,
         off_t offset,
         struct fuse_file_info* fi) {
    auto p = parsePath(path);
    handlers::read handler{buf, size, offset, fi};
    return boost::apply_visitor(handler, p);
}

}
