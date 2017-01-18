
#pragma once

#include <regex>
#include <string>
#include <utility>

namespace paths {

struct InvalidPath {
    auto tie() const { return std::tie(); }
    auto operator==(const InvalidPath& p) const { return tie() == p.tie(); }
};

struct DatabaseQueryPath {
    std::string query;
    auto tie() const { return std::tie(query); }
    auto operator==(const DatabaseQueryPath& p) const { return tie() == p.tie(); }
};

struct DevicePath {
    std::string device;
    auto tie() const { return std::tie(device); }
    auto operator==(const DevicePath& p) const { return tie() == p.tie(); }
};

struct DeviceClassPath {
    std::string device;
    auto tie() const { return std::tie(device); }
    auto operator==(const DeviceClassPath& p) const { return tie() == p.tie(); }
};

struct DeviceDescriptionPath {
    std::string device;
    auto tie() const { return std::tie(device); }
    auto operator==(const DeviceDescriptionPath& p) const { return tie() == p.tie(); }
};

struct DeviceNamePath {
    std::string device;
    auto tie() const { return std::tie(device); }
    auto operator==(const DeviceNamePath& p) const { return tie() == p.tie(); }
};

struct DeviceStatusPath {
    std::string device;
    auto tie() const { return std::tie(device); }
    auto operator==(const DeviceStatusPath& p) const { return tie() == p.tie(); }
};

struct DeviceAttributesPath {
    std::string device;
    auto tie() const { return std::tie(device); }
    auto operator==(const DeviceAttributesPath& p) const { return tie() == p.tie(); }
};

struct AttributePath {
    std::string device;
    std::string attribute;
    auto tie() const { return std::tie(device, attribute); }
    auto operator==(const AttributePath& p) const { return tie() == p.tie(); }
};

struct AttributeValuePath {
    std::string device;
    std::string attribute;
    auto tie() const { return std::tie(device, attribute); }
    auto operator==(const AttributeValuePath& p) const { return tie() == p.tie(); }
};

namespace __detail {

const std::string wordRegex = "[a-zA-Z-1-9\\._-]+";

constexpr auto re = [&](const std::string& s) {
    return std::regex(std::regex_replace(s, std::regex("%1%"), wordRegex));
};

const auto RE_ANY                = std::regex(".*");
const auto RE_ROOT               = std::regex("/?");
const auto RE_DOMAIN             = re("/(%1%)/?");
const auto RE_FAMILY             = re("/(%1%/%1%)/?");
const auto RE_DEVICE             = re("/(%1%/%1%/%1%)/?");
const auto RE_DEVICE_CLASS       = re("/(%1%/%1%/%1%)/class");
const auto RE_DEVICE_DESCRIPTION = re("/(%1%/%1%/%1%)/description");
const auto RE_DEVICE_NAME        = re("/(%1%/%1%/%1%)/name");
const auto RE_DEVICE_STATUS      = re("/(%1%/%1%/%1%)/status");
const auto RE_DEVICE_ATTRIBUTES  = re("/(%1%/%1%/%1%)/attributes/?");
const auto RE_ATTRIBUTE          = re("/(%1%/%1%/%1%)/attributes/(%1%)/?");
const auto RE_ATTRIBUTE_VALUE    = re("/(%1%/%1%/%1%)/attributes/(%1%)/value");

} // namespace __detail

constexpr auto makeFuseHandler = [](auto ...deps) {

    // TODO: forward lambda arguments
    // unfortunately, currently GCC does not allow for sth like
    // [args = std::forward<decltype(args)>(args)...](){}

    // TODO: explicit captures are required (probably) due to GCC bug
    // warning: 'args#-1' is used uninitialized in this function

    return [deps...](auto&& handler) {
    return [handler, deps...](auto path, auto ...args) {

        std::string p{path};
        std::smatch m{};

        auto match = [&](auto&&... a) {
            return regex_match(p, m, std::forward<decltype(a)>(a)...);
        };

        auto f = [&](auto&& p) {
            return handler(std::forward<decltype(p)>(p))(path, args...)(deps...);
        };

        using namespace __detail;

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

} // namespace paths
