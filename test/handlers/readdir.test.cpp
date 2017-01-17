
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <mocks.hpp>

#include <handlers/readdir.hpp>
#include <paths.hpp>

#include <string>
#include <utility>
#include <vector>

namespace handlers {

namespace {

struct FillerMock {

    MOCK_CONST_METHOD4(call, int(void*, std::string, void*, int));

    template <typename... Args>
    auto operator()(Args&&... args) const {
        return call(std::forward<Args>(args)...);
    }
};

}

using namespace ::testing;
using namespace std::string_literals;

struct HandlerReaddirTestSuite : Test {

    HandlerReaddirTestSuite()
        : databaseMock(stdx::in_place)
        , deviceProxyMock(stdx::in_place)
    {}

    void expectFillerCall(const std::string& s) {
        EXPECT_CALL(fillerMock, call(_, s,   _, 0)).WillOnce(Return(0));
    }

    template <typename Path>
    void testSuccessfulReaddir(Path&& path) {

        auto filler = [&](auto&&... args) {
            return this->fillerMock(std::forward<decltype(args)>(args)...);
        };

        auto result = readdir
            (std::forward<Path>(path))
            ("", nullptr, filler, 0, nullptr)
            (databaseProviderMock, deviceProxyProviderMock);

        ASSERT_EQ(0, result);
    }

    template <typename Path>
    void testUnsuccessfulReaddir(Path&& path) {

        ASSERT_EQ(-EIO, readdir(std::forward<Path>(path))(0)(0));
    }

    StrictMock<DatabaseProviderMock> databaseProviderMock{};
    StrictMock<DeviceProxyProviderMock> deviceProxyProviderMock{};

    StrictMock<FillerMock> fillerMock{};

    StrictMock<Maybe<DatabaseMock>> databaseMock;
    StrictMock<Maybe<DeviceProxyMock>> deviceProxyMock;
};

TEST_F(HandlerReaddirTestSuite, shouldFillDatabaseQuery) {

    StrictMock<DbDatumMock> dbDatumMock{};

    std::vector<std::string> queryResult = { "aaa/bbb/ccc",
                                             "aaa/ddd/eee",
                                             "aaa/fff/ggg",
                                             "aaa/ddd/hhh",
                                             "aaa/bbb/nnn" };

    EXPECT_CALL(dbDatumMock, toStringVector()).WillOnce(Return(queryResult));

    EXPECT_CALL(databaseProviderMock, call())
        .WillOnce(ReturnRef(databaseMock));

    EXPECT_CALL(*databaseMock, get_device_exported("aaa*"s))
        .WillOnce(ReturnRefOfCopy(DbDatumMockWrapper{dbDatumMock}));

    expectFillerCall(".");
    expectFillerCall("..");
    expectFillerCall("bbb");
    expectFillerCall("ddd");
    expectFillerCall("fff");

    testSuccessfulReaddir(paths::DatabaseQueryPath{"aaa"});
}

TEST_F(HandlerReaddirTestSuite, shouldFillDevicePath) {

    expectFillerCall(".");
    expectFillerCall("..");
    expectFillerCall("attributes");
    expectFillerCall("class");
    expectFillerCall("description");
    expectFillerCall("name");
    expectFillerCall("status");

    testSuccessfulReaddir(paths::DevicePath{"a/b/c"});
}

TEST_F(HandlerReaddirTestSuite, shouldFillDeviceAttributesPath) {

    auto attributeList = new std::vector<std::string>{ "bbb",
                                                       "ccc",
                                                       "bbb",
                                                       "ddd" };

    EXPECT_CALL(deviceProxyProviderMock, call("a/b/c"s))
        .WillOnce(ReturnRef(deviceProxyMock));

    EXPECT_CALL(*deviceProxyMock, get_attribute_list())
        .WillOnce(Return(attributeList));

    expectFillerCall(".");
    expectFillerCall("..");
    expectFillerCall("bbb");
    expectFillerCall("ccc");
    expectFillerCall("ddd");

    testSuccessfulReaddir(paths::DeviceAttributesPath{"a/b/c"});
}

TEST_F(HandlerReaddirTestSuite, shouldFillAttributePath) {

    expectFillerCall(".");
    expectFillerCall("..");
    expectFillerCall("value");

    testSuccessfulReaddir(paths::AttributePath{"a/b/c", "m"});
}

TEST_F(HandlerReaddirTestSuite, shouldNotFillInvalidPath) {
    testUnsuccessfulReaddir(paths::InvalidPath{});
}

TEST_F(HandlerReaddirTestSuite, shouldNotFillDeviceClassPath) {
    testUnsuccessfulReaddir(paths::DeviceClassPath{"a/b/c"});
}

TEST_F(HandlerReaddirTestSuite, shouldNotFillDeviceDescriptionPath) {
    testUnsuccessfulReaddir(paths::DeviceDescriptionPath{"a/b/c"});
}

TEST_F(HandlerReaddirTestSuite, shouldNotFillDeviceNamePath) {
    testUnsuccessfulReaddir(paths::DeviceNamePath{"a/b/c"});
}

TEST_F(HandlerReaddirTestSuite, shouldNotFillDeviceStatusPath) {
    testUnsuccessfulReaddir(paths::DeviceStatusPath{"a/b/c"});
}

TEST_F(HandlerReaddirTestSuite, shouldNotFillAttributeValuePath) {
    testUnsuccessfulReaddir(paths::AttributeValuePath{"a/b/c", "m"});
}

} // namespace handlers
