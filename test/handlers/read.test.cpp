
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

    template <typename Path>
    void testUnsuccessfulRead(Path&& path) {

        ASSERT_EQ(-EIO, read(std::forward<Path>(path))(0)(0));
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

    EXPECT_CALL(deviceProxyProviderMock, call(path))
        .WillOnce(ReturnRef(deviceProxyMock));

    EXPECT_CALL(*deviceProxyMock, info())
        .WillOnce(ReturnRefOfCopy(DeviceInfoMock{data}));

    testSuccessfulRead(paths::DeviceClassPath{path}, data);
}

TEST_F(HandlerReadTestSuite, shouldReadDeviceDescription) {

    auto path = "my/dev/1"s;
    auto data = "this is a device"s;

    EXPECT_CALL(deviceProxyProviderMock, call(path))
        .WillOnce(ReturnRef(deviceProxyMock));

    EXPECT_CALL(*deviceProxyMock, description()).WillOnce(Return(data));

    testSuccessfulRead(paths::DeviceDescriptionPath{path}, data);
}

TEST_F(HandlerReadTestSuite, shouldReadDeviceName) {

    auto path = "my/dev/1"s;
    auto data = path;

    EXPECT_CALL(deviceProxyProviderMock, call(path))
        .WillOnce(ReturnRef(deviceProxyMock));

    EXPECT_CALL(*deviceProxyMock, name()).WillOnce(Return(data));

    testSuccessfulRead(paths::DeviceNamePath{path}, data);
}

TEST_F(HandlerReadTestSuite, shouldReadDeviceStatus) {

    auto path = "my/dev/1"s;
    auto data = "device is in RUNNING state"s;

    EXPECT_CALL(deviceProxyProviderMock, call(path))
        .WillOnce(ReturnRef(deviceProxyMock));

    EXPECT_CALL(*deviceProxyMock, status()).WillOnce(Return(data));

    testSuccessfulRead(paths::DeviceStatusPath{path}, data);
}

TEST_F(HandlerReadTestSuite, shouldReadAttributeValue) {

    auto path = "my/dev/1"s;
    auto attr = "my_attribute"s;
    auto data = "some value"s;

    StrictMock<DeviceAttributeMock> attributeMock{};

    EXPECT_CALL(attributeMock, ostream(_))
        .WillOnce(Invoke([&](auto& s) { s << data; }));

    EXPECT_CALL(deviceProxyProviderMock, call(path))
        .WillOnce(ReturnRef(deviceProxyMock));

    EXPECT_CALL(*deviceProxyMock, read_attribute(attr))
        .WillOnce(ReturnRefOfCopy(DeviceAttributeMockWrapper{attributeMock}));

    testSuccessfulRead(paths::AttributeValuePath{path, attr}, data);
}

TEST_F(HandlerReadTestSuite, shouldNotReadFromInvalidPath) {
    testUnsuccessfulRead(paths::InvalidPath{});
}

TEST_F(HandlerReadTestSuite, shouldNotReadFromDatabaseQueryPath) {
    testUnsuccessfulRead(paths::DatabaseQueryPath{"path"});
}

TEST_F(HandlerReadTestSuite, shouldNotReadFromDevicePath) {
    testUnsuccessfulRead(paths::DevicePath{"a/b/c"});
}

TEST_F(HandlerReadTestSuite, shouldNotReadFromDeviceAttributesPath) {
    testUnsuccessfulRead(paths::DeviceAttributesPath{"a/b/c"});
}

TEST_F(HandlerReadTestSuite, shouldNotReadFromAttributePath) {
    testUnsuccessfulRead(paths::AttributePath{"a/b/c", "m"});
}

TEST_F(HandlerReadTestSuite, shouldReadStartingFromOffset) {

    auto path = "my/dev/1"s;
    auto data = "longdata"s;

    EXPECT_CALL(deviceProxyProviderMock, call(path))
        .WillOnce(ReturnRef(deviceProxyMock));

    EXPECT_CALL(*deviceProxyMock, name()).WillOnce(Return(data));

    auto readSize = read
        (paths::DeviceNamePath{path})
        ("", buffer, bufferSize, 3, 0)
        (databaseProviderMock, deviceProxyProviderMock);

    ASSERT_EQ("gdata\n", std::string(buffer, readSize));
}

TEST_F(HandlerReadTestSuite, shouldReadNothingIfOffsetExceedsLength) {

    auto path = "my/dev/1"s;
    auto data = "data"s;

    EXPECT_CALL(deviceProxyProviderMock, call(path))
        .WillOnce(ReturnRef(deviceProxyMock));

    EXPECT_CALL(*deviceProxyMock, name()).WillOnce(Return(data));

    auto readSize = read
        (paths::DeviceNamePath{path})
        ("", buffer, bufferSize, data.size() + 1, 0)
        (databaseProviderMock, deviceProxyProviderMock);

    ASSERT_EQ(0, readSize);
}

} // namespace handlers
