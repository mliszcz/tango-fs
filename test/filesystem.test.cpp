
#include <boost/test/unit_test.hpp>

#include <filesystem.hpp>

BOOST_AUTO_TEST_SUITE(filesystem)

BOOST_AUTO_TEST_CASE(ShouldHandleRootPath)
{
    auto path = filesystem::parsePath("/");
    auto p = boost::get<paths::DatabaseQueryPath>(path);
    BOOST_TEST(p.query == "");
}

BOOST_AUTO_TEST_CASE(ShouldHandleDomainPath)
{
    auto path = filesystem::parsePath("/domain-.123");
    auto p = boost::get<paths::DatabaseQueryPath>(path);
    BOOST_TEST(p.query == "domain-.123");
}

BOOST_AUTO_TEST_CASE(ShouldHandleFamilyPath)
{
    auto path = filesystem::parsePath("/domain/family");
    auto p = boost::get<paths::DatabaseQueryPath>(path);
    BOOST_TEST(p.query == "domain/family");
}

BOOST_AUTO_TEST_CASE(ShouldHandleDevicePath)
{
    auto path = filesystem::parsePath("/sys/tg-test/1");
    auto p = boost::get<paths::DevicePath>(path);
    BOOST_TEST(p.device == "sys/tg-test/1");
}

BOOST_AUTO_TEST_CASE(ShouldHandleDeviceClassPath)
{
    auto path = filesystem::parsePath("/a/b/c/class");
    auto p = boost::get<paths::DeviceClassPath>(path);
    BOOST_TEST(p.device == "a/b/c");
}

//
//
// struct DeviceClassPath {
//     std::string device;
// };
//
// struct DeviceDescriptionPath {
//     std::string device;
// };
//
// struct DeviceNamePath {
//     std::string device;
// };
//
// struct DeviceStatusPath {
//     std::string device;
// };


BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES(TestThatFails, 1)

BOOST_AUTO_TEST_CASE(TestThatFails)
{
    BOOST_CHECK(false);
}

BOOST_AUTO_TEST_SUITE_END()
