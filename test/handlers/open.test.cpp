
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <handlers/open.hpp>
#include <paths.hpp>

namespace handlers {

using namespace ::testing;

struct HandlerOpenTestSuite : Test {};

TEST_F(HandlerOpenTestSuite, shouldReturnZeroAndIgnoreArguments) {
    ASSERT_EQ(0, open(paths::DatabaseQueryPath{""})(5)(10, 15));
}

} // namespace handlers
