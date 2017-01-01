
#pragma once

#include <types.hpp>

#include <tango.h>

#include <boost/optional.hpp>
#include <string>

namespace tango {

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
T extractFromDbDatum(Tango::DbDatum dbDatum) {
    T t{};
    dbDatum >> t;
    return t;
}

}
