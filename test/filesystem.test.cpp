
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(filesystem)

BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES(TestThatFails, 1)

BOOST_AUTO_TEST_CASE(TestThatFails)
{
    BOOST_CHECK(false);
}

BOOST_AUTO_TEST_SUITE_END()
