
#pragma once

#include <paths.hpp>
#include <tango.hpp>

#include <set>
#include <string>

namespace lookup {

inline auto directoryEntries(const paths::DatabaseQueryPath& path) {
    return [=](auto&& database, auto&&) {
        return tango::findDirectChildrenInDatabase(path.query)(database());
    };
}

inline auto directoryEntries(const paths::DeviceAttributesPath& path) {
    return [=](auto&&, auto&& deviceProxy) {
        auto proxy = deviceProxy(path.device);
        return tango::getDeviceAttributeList(proxy);
    };
}

inline auto directoryEntries(const paths::DevicePath&) {
    return [=](auto&&...) {
        return std::set<std::string>{"attributes",
                                     "class",
                                     "description",
                                     "name",
                                     "status"};
    };
}

inline auto directoryEntries(const paths::AttributePath&) {
    return [=](auto&&...) {
        return std::set<std::string>{"value"};
    };
}

inline auto fileContents(const paths::DeviceClassPath& path) {
    return [=](auto&&, auto&& deviceProxy) {
        auto f = [](auto& p){ return p.info().dev_class; };
        return tango::extractFromDeviceProxy(f)(deviceProxy(path.device));
    };
}

inline auto fileContents(const paths::DeviceDescriptionPath& path) {
    return [=](auto&&, auto&& deviceProxy) {
        auto f = std::mem_fn(&Tango::DeviceProxy::description);
        return tango::extractFromDeviceProxy(f)(deviceProxy(path.device));
    };
}

inline auto fileContents(const paths::DeviceNamePath& path) {
    return [=](auto&&, auto&& deviceProxy) {
        auto f = std::mem_fn(&Tango::DeviceProxy::name);
        return tango::extractFromDeviceProxy(f)(deviceProxy(path.device));
    };
}

inline auto fileContents(const paths::DeviceStatusPath& path) {
    return [=](auto&&, auto&& deviceProxy) {
        auto f = std::mem_fn(&Tango::DeviceProxy::status);
        return tango::extractFromDeviceProxy(f)(deviceProxy(path.device));
    };
}

inline auto fileContents(const paths::AttributeValuePath& path) {
    return [=](auto&&, auto&& deviceProxy) {
        return tango::extractFromDeviceAttribute
            (tango::extractStringFromDeviceAttribute)
            (path.attribute)
            (deviceProxy(path.device));
    };
}

} // namespace lookup
