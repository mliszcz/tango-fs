
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <mocks.hpp>

#include <handlers/getattr.hpp>
#include <paths.hpp>

#include <string>
#include <utility>

namespace handlers {

using namespace ::testing;

struct HandlerGetattrTestSuite : Test {

    HandlerGetattrTestSuite()
        : databaseMock(stdx::in_place)
        , deviceProxyMock(stdx::in_place)
    {}

    template <typename Path>
    void testSuccessfulReadAsDirectory(Path&& path, int entries) {

        struct stat stbuf{};

        auto result = getattr
            (std::forward<Path>(path))
            ("", &stbuf)
            (databaseProviderMock, deviceProxyProviderMock);

        ASSERT_EQ(0, result);
        ASSERT_EQ(entries + 2, stbuf.st_nlink);
        ASSERT_TRUE(stbuf.st_mode & S_IFDIR);
    }

    template <typename Path>
    void testSuccessfulReadAsFile(Path&& path, int size) {

        struct stat stbuf{};

        auto result = getattr
            (std::forward<Path>(path))
            ("", &stbuf)
            (databaseProviderMock, deviceProxyProviderMock);

        ASSERT_EQ(0, result);
        ASSERT_EQ(1, stbuf.st_nlink);
        ASSERT_EQ(size + 1, stbuf.st_size);
        ASSERT_TRUE(stbuf.st_mode & S_IFREG);
    }

    StrictMock<DatabaseProviderMock> databaseProviderMock{};
    StrictMock<DeviceProxyProviderMock> deviceProxyProviderMock{};

    StrictMock<Maybe<DatabaseMock>> databaseMock;
    StrictMock<Maybe<DeviceProxyMock>> deviceProxyMock;
};

TEST_F(HandlerGetattrTestSuite, shouldHandleInvalidPath) {
    struct stat* stbuf{};
    ASSERT_EQ(-ENOENT, getattr(paths::InvalidPath{})("", &stbuf)(0));
}

TEST_F(HandlerGetattrTestSuite, shouldHandleDatabaseQueryPath) {

    StrictMock<DbDatumMock> dbDatumMock{};

    std::vector<std::string> result = { "a/b/c", "a/d/e" };

    EXPECT_CALL(dbDatumMock, toStringVector()).WillOnce(Return(result));

    EXPECT_CALL(databaseProviderMock, call())
        .WillOnce(ReturnRef(databaseMock));

    EXPECT_CALL(*databaseMock, get_device_exported("a*"s))
        .WillOnce(ReturnRefOfCopy(DbDatumMockWrapper{dbDatumMock}));

    testSuccessfulReadAsDirectory(paths::DatabaseQueryPath{"a"}, result.size());
}

TEST_F(HandlerGetattrTestSuite, shouldHandleDevicePath) {
    const auto ENTRIES = 5; // attributes, class, description, name, status
    testSuccessfulReadAsDirectory(paths::DevicePath{"a/b/c"}, ENTRIES);
}

TEST_F(HandlerGetattrTestSuite, shouldHandleDeviceAttributesPath) {

    auto attributeList = new std::vector<std::string>{ "p", "q", "r" };

    EXPECT_CALL(deviceProxyProviderMock, call("a/b/c"s))
        .WillOnce(ReturnRef(deviceProxyMock));

    EXPECT_CALL(*deviceProxyMock, get_attribute_list())
        .WillOnce(Return(attributeList));

    testSuccessfulReadAsDirectory(paths::DeviceAttributesPath{"a/b/c"},
                                  attributeList->size());
}

TEST_F(HandlerGetattrTestSuite, shouldHandleDeviceClassPath) {

    auto path = "my/dev/1"s;
    auto data = "clazz"s;

    EXPECT_CALL(deviceProxyProviderMock, call(path))
        .WillOnce(ReturnRef(deviceProxyMock));

    EXPECT_CALL(*deviceProxyMock, info())
        .WillOnce(ReturnRefOfCopy(DeviceInfoMock{data}));

    testSuccessfulReadAsFile(paths::DeviceClassPath{path}, data.size());
}

TEST_F(HandlerGetattrTestSuite, shouldHandleDeviceDescriptionPath) {

    auto path = "my/dev/1"s;
    auto data = "this is a device"s;

    EXPECT_CALL(deviceProxyProviderMock, call(path))
        .WillOnce(ReturnRef(deviceProxyMock));

    EXPECT_CALL(*deviceProxyMock, description()).WillOnce(Return(data));

    testSuccessfulReadAsFile(paths::DeviceDescriptionPath{path}, data.size());
}

TEST_F(HandlerGetattrTestSuite, shouldHandleDeviceNamePath) {

    auto path = "my/dev/1"s;
    auto data = path;

    EXPECT_CALL(deviceProxyProviderMock, call(path))
        .WillOnce(ReturnRef(deviceProxyMock));

    EXPECT_CALL(*deviceProxyMock, name()).WillOnce(Return(data));

    testSuccessfulReadAsFile(paths::DeviceNamePath{path}, data.size());
}

TEST_F(HandlerGetattrTestSuite, shouldHandleDeviceStatusPath) {

    auto path = "my/dev/1"s;
    auto data = "device is in RUNNING state"s;

    EXPECT_CALL(deviceProxyProviderMock, call(path))
        .WillOnce(ReturnRef(deviceProxyMock));

    EXPECT_CALL(*deviceProxyMock, status()).WillOnce(Return(data));

    testSuccessfulReadAsFile(paths::DeviceStatusPath{path}, data.size());
}

TEST_F(HandlerGetattrTestSuite, shouldHandleAttributePath) {
    const auto ENTRIES = 1; // value
    testSuccessfulReadAsDirectory(paths::AttributePath{"a/b/c", "m"}, ENTRIES);
}

TEST_F(HandlerGetattrTestSuite, shouldHandleAttributeValuePath) {

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

    testSuccessfulReadAsFile(paths::AttributeValuePath{path, attr}, data.size());
}

} // namespace handlers
