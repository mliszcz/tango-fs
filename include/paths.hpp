
#pragma once

#include <string>
#include <boost/variant.hpp>

namespace paths {

struct InvalidPath {};

struct DatabaseQueryPath {
    std::string query;
};

struct DevicePath {
    std::string device;
};

struct DeviceClassPath {
    std::string device;
};

struct DeviceDescriptionPath {
    std::string device;
};

struct DeviceNamePath {
    std::string device;
};

struct DeviceStatusPath {
    std::string device;
};

struct DeviceAttributesPath {
    std::string device;
};

struct AttributePath {
    std::string attribute;
};

using AnyPath = boost::variant<
    InvalidPath,
    DatabaseQueryPath,
    DevicePath,
    DeviceClassPath,
    DeviceDescriptionPath,
    DeviceNamePath,
    DeviceStatusPath,
    DeviceAttributesPath,
    AttributePath>;
}
