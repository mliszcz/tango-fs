
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <paths.hpp>

namespace paths {

using namespace ::testing;

namespace {

struct TangoDependneciesCallbackMock {
    MOCK_CONST_METHOD1(call, int(int));

    template <typename... Args>
    auto operator()(Args&&... args) const {
        return call(std::forward<Args>(args)...);
    }
};

struct FuseInterfaceCallbackMock {
    MOCK_CONST_METHOD2(call, TangoDependneciesCallbackMock&(const char*, int));

    template <typename... Args>
    auto& operator()(Args&&... args) const {
        return call(std::forward<Args>(args)...);
    }
};

struct PathCallbackMock {
    MOCK_CONST_METHOD1(call, FuseInterfaceCallbackMock&(const DatabaseQueryPath&));
    MOCK_CONST_METHOD1(call, FuseInterfaceCallbackMock&(const DevicePath&));
    MOCK_CONST_METHOD1(call, FuseInterfaceCallbackMock&(const DeviceClassPath&));
    MOCK_CONST_METHOD1(call, FuseInterfaceCallbackMock&(const DeviceDescriptionPath&));
    MOCK_CONST_METHOD1(call, FuseInterfaceCallbackMock&(const DeviceNamePath&));
    MOCK_CONST_METHOD1(call, FuseInterfaceCallbackMock&(const DeviceStatusPath&));
    MOCK_CONST_METHOD1(call, FuseInterfaceCallbackMock&(const DeviceAttributesPath&));
    MOCK_CONST_METHOD1(call, FuseInterfaceCallbackMock&(const AttributePath&));
    MOCK_CONST_METHOD1(call, FuseInterfaceCallbackMock&(const AttributeValuePath&));
    MOCK_CONST_METHOD1(call, FuseInterfaceCallbackMock&(const InvalidPath&));

    template <typename... Args>
    auto& operator()(Args&&... args) const {
        return call(std::forward<Args>(args)...);
    }
};

} // namespace

struct PathsTestSuite : Test {

    StrictMock<TangoDependneciesCallbackMock> tangoDependenciesCallback{};
    StrictMock<FuseInterfaceCallbackMock> fuseInterfaceCallback{};
    StrictMock<PathCallbackMock> pathCallback{};

    template <typename Path>
    void testFuseHandler(const char* fusePath, const Path& tangoPath) {

        const auto FINAL_RESULT = 7;
        const auto FUSE_PARAM = 11;
        const auto TANGO_DEP = 23;

        EXPECT_CALL(tangoDependenciesCallback, call(TANGO_DEP))
            .WillOnce(Return(FINAL_RESULT));
        EXPECT_CALL(fuseInterfaceCallback, call(_, FUSE_PARAM))
            .WillOnce(ReturnRef(tangoDependenciesCallback));
        EXPECT_CALL(pathCallback, call(tangoPath))
            .WillOnce(ReturnRef(fuseInterfaceCallback));

        auto f = [&](auto&& p) -> FuseInterfaceCallbackMock& {
            return this->pathCallback(std::forward<decltype(p)>(p));
        };

        auto result = makeFuseHandler(TANGO_DEP)(f)(fusePath, FUSE_PARAM);

        ASSERT_EQ(FINAL_RESULT, result);
    }

};

TEST_F(PathsTestSuite, shouldHandleRootPath) {
    testFuseHandler("/", DatabaseQueryPath{""});
}

TEST_F(PathsTestSuite, shouldHandleDomainPath) {
    testFuseHandler("/domain", DatabaseQueryPath{"domain"});
}

TEST_F(PathsTestSuite, shouldHandleFamilyPath) {
    testFuseHandler("/domain/family", DatabaseQueryPath{"domain/family"});
}

TEST_F(PathsTestSuite, shouldHandleDevicePath) {
    testFuseHandler("/aa/bbb/cccc", DevicePath{"aa/bbb/cccc"});
}

TEST_F(PathsTestSuite, shouldHandleDeviceAttributesPath) {
    testFuseHandler("/a/b/c/attributes", DeviceAttributesPath{"a/b/c"});
}

TEST_F(PathsTestSuite, shouldHandleDeviceClassPath) {
    testFuseHandler("/a/b/c/class", DeviceClassPath{"a/b/c"});
}

TEST_F(PathsTestSuite, shouldHandleDeviceDescriptionPath) {
    testFuseHandler("/a/b/c/description", DeviceDescriptionPath{"a/b/c"});
}

TEST_F(PathsTestSuite, shouldHandleDeviceNamePath) {
    testFuseHandler("/a/b/c/name", DeviceNamePath{"a/b/c"});
}

TEST_F(PathsTestSuite, shouldHandleDeviceStatusPath) {
    testFuseHandler("/a/b/c/status", DeviceStatusPath{"a/b/c"});
}

TEST_F(PathsTestSuite, shouldHandleAttributePath) {
    testFuseHandler("/a/b/c/attributes/m", AttributePath{"a/b/c", "m"});
}

TEST_F(PathsTestSuite, shouldHandleAttributeValuePath) {
    testFuseHandler("/a/b/c/attributes/m/value",
                    AttributeValuePath{"a/b/c", "m"});
}

TEST_F(PathsTestSuite, shouldHandleSpecialCharactersInPaths) {
    testFuseHandler("/aA1_-./2.B-_b/-3C.c_", DevicePath{"aA1_-./2.B-_b/-3C.c_"});
}

TEST_F(PathsTestSuite, shouldIgnoreTrailingSlash) {
    testFuseHandler("/domain/family/", DatabaseQueryPath{"domain/family"});
}

TEST_F(PathsTestSuite, shouldRejectRelativePath) {
    testFuseHandler("domain/family", InvalidPath{});
}

TEST_F(PathsTestSuite, shouldRejectInvalidDeviceSubdirectoryPath) {
    testFuseHandler("/a/b/c/unknown", InvalidPath{});
}

TEST_F(PathsTestSuite, shouldRejectInvalidAttributeSubdirectoryPath) {
    testFuseHandler("/a/b/c/attributes/m/undefined", InvalidPath{});
}

TEST_F(PathsTestSuite, shouldRejectTooLongPaths) {
    testFuseHandler("/a/b/c/attributes/m/value/p/q/r", InvalidPath{});
}

} // namespace paths
