
#pragma once

#include <tango.hpp>

#include <boost/optional.hpp>
#include <set>
#include <string>

namespace lookup {

template <typename Path>
auto directoryEntries(const Path&) {
    return std::set<std::string>{};
}

auto directoryEntries(const paths::DatabaseQueryPath& path) {
    auto database = tango::createDatabase();
    return tango::findDirectChildrenInDatabase(path.query)(database);
}

auto directoryEntries(const paths::DeviceAttributesPath& path) {
    auto proxy = tango::createDeviceProxy(path.device);
    return tango::getDeviceAttributeList(proxy);
}

auto directoryEntries(const paths::DevicePath&) {
    return tango::getDeviceSubdirectories();
}

template <typename Path>
auto fileContents(const Path&) {
    return boost::optional<std::string>(boost::none);
}

auto fileContents(const paths::DeviceClassPath& path) {
    auto proxy = tango::createDeviceProxy(path.device);
    auto f = [](auto& p){ return p.info().dev_class; };
    return tango::extractFromDeviceProxy(f)(proxy);
}

auto fileContents(const paths::DeviceDescriptionPath& path) {
    auto proxy = tango::createDeviceProxy(path.device);
    auto f = std::mem_fn(&Tango::DeviceProxy::description);
    return tango::extractFromDeviceProxy(f)(proxy);
}

auto fileContents(const paths::DeviceNamePath& path) {
    auto proxy = tango::createDeviceProxy(path.device);
    auto f = std::mem_fn(&Tango::DeviceProxy::name);
    return tango::extractFromDeviceProxy(f)(proxy);
}

auto fileContents(const paths::DeviceStatusPath& path) {
    auto proxy = tango::createDeviceProxy(path.device);
    auto f = std::mem_fn(&Tango::DeviceProxy::status);
    return tango::extractFromDeviceProxy(f)(proxy);
}

}
