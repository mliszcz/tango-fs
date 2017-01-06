
#pragma once

#include <types.hpp>

#include <tango.h>

#include <boost/optional.hpp>
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace tango {

namespace __detail {

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

constexpr auto appendTrailingNewline = [](const auto& s) { return s + "\n"; };

}

Maybe<Tango::DeviceProxy> createDeviceProxy(std::string path) {
    return Just(path) >= [](auto s){ return Tango::DeviceProxy(s); };
}

Maybe<Tango::Database> createDatabase() {
    try {
        return Tango::Database();
    }  catch (...) {
        return boost::none;
    }
}

template <typename T>
constexpr auto extractFromDbDatum = [](auto&& dbDatum) {
    T t{};
    dbDatum >> t;
    return t;
};

constexpr auto extractFromDeviceProxy = [](auto&& f) {
    return [=](auto&& proxy) {
        return proxy >= f >= __detail::appendTrailingNewline;
    };
};

constexpr auto findDirectChildrenInDatabase = [](auto&& path) {
    return [=](auto&& database) {

        auto childrenPath = path + "*";

        auto result = (database
            >= [&](auto& db){ return db.get_device_exported(childrenPath); }
            >= extractFromDbDatum<std::vector<std::string>>)
            .get_value_or({});

        // + 1 to drop leading slash (root entries do not have this)
        auto prefix = path.empty() ? 0 : (path.size() + 1);

        auto entries = result
            >= __detail::removeStringPrefix(prefix)
            >= __detail::removeStringAfter('/');

        return std::set<std::string>(entries.begin(), entries.end());
    };
};

auto getDeviceSubdirectories() {
    return std::set<std::string>{"attributes",
                                 "class",
                                 "description",
                                 "name",
                                 "status"};
}

constexpr auto getDeviceAttributeList = [](auto&& proxy) {

    using V = std::vector<std::string>;

    auto maybeData = proxy
        >= [](auto& p){ return std::shared_ptr<V>(p.get_attribute_list()); };

    auto data = maybeData.get_value_or(std::make_shared<V>());

    return std::set<std::string>(data->begin(), data->end());
};

}
