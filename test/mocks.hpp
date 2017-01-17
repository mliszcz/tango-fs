
#pragma once

#include <types.hpp>

#include <iostream>
#include <utility>
#include <string>

struct DbDatumMock {
    MOCK_CONST_METHOD0(toStringVector, std::vector<std::string>());
};

struct DbDatumMockWrapper {
    DbDatumMock& mock;
};

inline void operator>>(const DbDatumMockWrapper& w,
                       std::vector<std::string>& s) {
    s = w.mock.toStringVector();
}

struct DeviceAttributeMock {
    MOCK_CONST_METHOD1(ostream, void(std::ostream&));
};

struct DeviceAttributeMockWrapper {
    DeviceAttributeMock& mock;
};

inline std::ostream& operator<<(std::ostream& s,
                                const DeviceAttributeMockWrapper& w) {
    w.mock.ostream(s);
    return s;
}

struct DeviceInfoMock {
    std::string dev_class;
};

struct DatabaseMock {
    MOCK_CONST_METHOD1(get_device_exported, DbDatumMockWrapper&(std::string));
};

struct DeviceProxyMock {
    MOCK_CONST_METHOD1(read_attribute, DeviceAttributeMockWrapper&(std::string));
    MOCK_CONST_METHOD0(get_attribute_list, std::vector<std::string>*());
    MOCK_CONST_METHOD0(info, DeviceInfoMock&());
    MOCK_CONST_METHOD0(description, std::string());
    MOCK_CONST_METHOD0(name, std::string());
    MOCK_CONST_METHOD0(status, std::string());
};

struct DatabaseProviderMock {

    MOCK_CONST_METHOD0(call, Maybe<DatabaseMock>&());

    template<typename... Args>
    auto& operator()(Args&&... args) const {
        return call(std::forward<Args>(args)...);
    }
};

struct DeviceProxyProviderMock {

    MOCK_CONST_METHOD1(call, Maybe<DeviceProxyMock>&(std::string));

    template<typename... Args>
    auto& operator()(Args&&... args) const {
        return call(std::forward<Args>(args)...);
    }
};
