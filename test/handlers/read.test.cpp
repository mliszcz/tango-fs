
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <mocks.hpp>

#include <handlers/read.hpp>
#include <paths.hpp>

#include <string>
#include <utility>

namespace handlers {

using namespace ::testing;
using namespace std::string_literals;

struct HandlerReadTestSuite : Test {

    HandlerReadTestSuite()
        : databaseMock(stdx::in_place)
        , deviceProxyMock(stdx::in_place)
    {}

    template <typename Path>
    void testSuccessfulRead(Path&& path, const auto& expectedData) {

        auto readSize = read
            (std::forward<Path>(path))
            ("", buffer, bufferSize, 0, 0)
            (databaseProviderMock, deviceProxyProviderMock);

        ASSERT_EQ(expectedData + "\n", std::string(buffer, readSize));
    }

    char buffer[128];
    int bufferSize{sizeof(buffer)};

    StrictMock<DatabaseProviderMock> databaseProviderMock{};
    StrictMock<DeviceProxyProviderMock> deviceProxyProviderMock{};

    StrictMock<Maybe<DatabaseMock>> databaseMock;
    StrictMock<Maybe<DeviceProxyMock>> deviceProxyMock;
};

TEST_F(HandlerReadTestSuite, shouldReadDeviceClass) {

    auto path = "my/dev/1"s;
    auto data = "clazz"s;

    EXPECT_CALL(deviceProxyProviderMock, call({path}))
        .WillOnce(ReturnRef(deviceProxyMock));

    EXPECT_CALL(*deviceProxyMock, info())
        .WillOnce(ReturnRefOfCopy(DeviceInfoMock{data}));

    testSuccessfulRead(paths::DeviceClassPath{path}, data);
}

TEST_F(HandlerReadTestSuite, shouldReadDeviceDescription) {

    auto path = "my/dev/1"s;
    auto data = "this is a device"s;

    EXPECT_CALL(deviceProxyProviderMock, call({path}))
        .WillOnce(ReturnRef(deviceProxyMock));

    EXPECT_CALL(*deviceProxyMock, description()).WillOnce(Return(data));

    testSuccessfulRead(paths::DeviceDescriptionPath{path}, data);
}

TEST_F(HandlerReadTestSuite, shouldReadDeviceName) {

    auto path = "my/dev/1"s;
    auto data = path;

    EXPECT_CALL(deviceProxyProviderMock, call({path}))
        .WillOnce(ReturnRef(deviceProxyMock));

    EXPECT_CALL(*deviceProxyMock, name()).WillOnce(Return(data));

    testSuccessfulRead(paths::DeviceNamePath{path}, data);
}

TEST_F(HandlerReadTestSuite, shouldReadDeviceStatus) {

    auto path = "my/dev/1"s;
    auto data = "device is in RUNNING state"s;

    EXPECT_CALL(deviceProxyProviderMock, call({path}))
        .WillOnce(ReturnRef(deviceProxyMock));

    EXPECT_CALL(*deviceProxyMock, status()).WillOnce(Return(data));

    testSuccessfulRead(paths::DeviceStatusPath{path}, data);
}

// TEST_F(HandlerReadTestSuite, shouldReadAttributeValue) {
//
//     auto path = "my/dev/1"s;
//     auto attr = "my_attribute"s;
//     auto data = "some value"s;
//
//     StrictMock<DbDatumMock> dbDatumMock{};
//
//     EXPECT_CALL(dbDatumMock, ostream(_))
//         .WillOnce(Invoke([&](auto& s) { s << data; }));
//
//     EXPECT_CALL(deviceProxyProviderMock, call({path}))
//         .WillOnce(ReturnRef(deviceProxyMock));
//
//     EXPECT_CALL(*deviceProxyMock, read_attribute(_))
//         // .WillOnce(ReturnRef(dbDatumMock));
//         .WillOnce(ReturnRef(dbDatumMock));
//
//     testSuccessfulRead(paths::AttributeValuePath{path, attr}, data);
// }


} // namespace handlers
