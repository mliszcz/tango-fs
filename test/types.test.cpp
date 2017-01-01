
#include <boost/test/unit_test.hpp>

#include <types.hpp>

BOOST_AUTO_TEST_SUITE(types)

namespace {
    struct A{};
    struct B{};
    struct C{};

    template <typename T, typename = std::enable_if_t<is_any_same<T,A,B>::value>>
    void callIfAOrB(T t) {};
}

BOOST_AUTO_TEST_CASE(ShouldEnableFunctionForSupportedTypes)
{
    callIfAOrB(A{});
    callIfAOrB(B{});
}

BOOST_AUTO_TEST_SUITE_END()
