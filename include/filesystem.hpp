
#pragma once

#include <paths.hpp>
#include <handlers/getattr.hpp>
#include <boost/variant.hpp>
#include <boost/format.hpp>
#include <regex>
#include <utility>

namespace filesystem {

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
    auto RE_DEVICE_ATTRIBUTES  = re("/(%1%/%1%/%1%)/attributes");

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

    return InvalidPath{};
}

int getattr(const char* path, struct stat* stbuf) {
    auto p = parsePath(path);
    handlers::getattr handler{stbuf};
    return boost::apply_visitor(handler, p);
}

}
