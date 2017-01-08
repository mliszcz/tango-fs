
#pragma once

#include <tango.hpp>

#include <boost/optional.hpp>
#include <set>
#include <string>

namespace lookup {

/**
 * Path -> (-> Database), (string -> DeviceProxy) -> set<string>
 */
template <typename Path>
auto directoryEntries(const Path&) {
    return [=](auto&&, auto&&) {
        return std::set<std::string>{};
    };
}

auto directoryEntries(const paths::DatabaseQueryPath& path) {
    return [=](auto&& database, auto&&) {
        return tango::findDirectChildrenInDatabase(path.query)(database());
    };
}

auto directoryEntries(const paths::DeviceAttributesPath& path) {
    return [=](auto&&, auto&& deviceProxy) {
        auto proxy = deviceProxy(path.device);
        return tango::getDeviceAttributeList(proxy);
    };
}

auto directoryEntries(const paths::DevicePath&) {
    return [=](auto&&, auto&&) {
        return std::set<std::string>{"attributes",
                                     "class",
                                     "description",
                                     "name",
                                     "status"};
    };
}

auto directoryEntries(const paths::AttributePath&) {
    return [=](auto&&, auto&&) {
        return std::set<std::string>{"value"};
    };
}

/**
 * Path -> (-> Database), (string -> DeviceProxy) -> Maybe<string>
 */
template <typename Path>
auto fileContents(const Path&) {
    return [=](auto&&, auto&&) {
        return boost::optional<std::string>(boost::none);
    };
}

auto fileContents(const paths::DeviceClassPath& path) {
    return [=](auto&&, auto&& deviceProxy) {
        auto f = [](auto& p){ return p.info().dev_class; };
        return tango::extractFromDeviceProxy(f)(deviceProxy(path.device));
    };
}

auto fileContents(const paths::DeviceDescriptionPath& path) {
    return [=](auto&&, auto&& deviceProxy) {
        auto f = std::mem_fn(&Tango::DeviceProxy::description);
        return tango::extractFromDeviceProxy(f)(deviceProxy(path.device));
    };
}

auto fileContents(const paths::DeviceNamePath& path) {
    return [=](auto&&, auto&& deviceProxy) {
        auto f = std::mem_fn(&Tango::DeviceProxy::name);
        return tango::extractFromDeviceProxy(f)(deviceProxy(path.device));
    };
}

auto fileContents(const paths::DeviceStatusPath& path) {
    return [=](auto&&, auto&& deviceProxy) {
        auto f = std::mem_fn(&Tango::DeviceProxy::status);
        return tango::extractFromDeviceProxy(f)(deviceProxy(path.device));
    };
}

auto fileContents(const paths::AttributeValuePath& path) {
    return [=](auto&&, auto&& deviceProxy) {
        return tango::extractFromDeviceAttribute
            (tango::extractStringFromDeviceAttribute)
            (path.attribute)
            (deviceProxy(path.device));
    };
}

}
