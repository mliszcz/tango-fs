
#define FUSE_USE_VERSION 30

#include <handlers/getattr.hpp>
#include <handlers/readdir.hpp>
#include <handlers/open.hpp>
#include <handlers/read.hpp>
#include <paths.hpp>
#include <tango.hpp>

#include <fuse.h>

#include <utility>

namespace {

const auto handler = paths::makeFuseHandler(tango::createDatabase,
                                            tango::createDeviceProxy);

constexpr auto getattr = [](auto&& p) {
    return handlers::getattr(std::forward<decltype(p)>(p));
};

constexpr auto open_ = [](auto&& p) {
    return handlers::open(std::forward<decltype(p)>(p));
};

constexpr auto read_ = [](auto&& p) {
    return handlers::read(std::forward<decltype(p)>(p));
};

constexpr auto readdir = [](auto&& p) {
    return handlers::readdir(std::forward<decltype(p)>(p));
};

} // namespace

int main(int argc, char *argv[])
{
    struct fuse_operations ops{};
    ops.getattr = [](auto... args) { return handler(getattr)(args...); };
    ops.open    = [](auto... args) { return handler(open_)(args...); };
    ops.read    = [](auto... args) { return handler(read_)(args...); };
    ops.readdir = [](auto... args) { return handler(readdir)(args...); };

    return fuse_main(argc, argv, &ops, nullptr);
}
