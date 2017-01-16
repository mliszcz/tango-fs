
#pragma once

#include <types.hpp>

#include <iostream>
#include <utility>
#include <string>

template <typename Ret, typename... Args>
struct FunctionMock {

    MOCK_CONST_METHOD1_T(call, Ret(std::tuple<Args...>));

    Ret operator()(Args... args) const {
        return call(std::make_tuple(args...));
    }
};

struct DeviceAttributeMock {
    MOCK_CONST_METHOD1(ostream, void(std::ostream&));
};

std::ostream& operator<<(std::ostream& s, const DeviceAttributeMock& m) {
    m.ostream(s);
    return s;
}

struct DeviceAttributeMockWrapper {
    DeviceAttributeMock& mock;
};

std::ostream& operator<<(std::ostream& s, const DeviceAttributeMockWrapper& w) {
    return s << w.mock;
}

struct DeviceInfoMock {
    std::string dev_class;
};

struct DatabaseMock {};

struct DeviceProxyMock {
    MOCK_CONST_METHOD1(read_attribute, DeviceAttributeMockWrapper&(const char*));
    MOCK_CONST_METHOD0(info, DeviceInfoMock&());
    MOCK_CONST_METHOD0(description, std::string());
    MOCK_CONST_METHOD0(name, std::string());
    MOCK_CONST_METHOD0(status, std::string());
};

using DatabaseProviderMock
    = FunctionMock<Maybe<DatabaseMock>&>;

using DeviceProxyProviderMock
    = FunctionMock<Maybe<DeviceProxyMock>&, std::string>;
