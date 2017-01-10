
#define FUSE_USE_VERSION 30

#include <filesystem.hpp>

namespace {

const auto handler = filesystem::makeFuseHandler(tango::createDatabase,
                                                 tango::createDeviceProxy);

constexpr auto getattr = [](auto&& p) {
    return handlers::getattr(std::forward<decltype(p)>(p));
};

}

int main(int argc, char *argv[])
{

    struct fuse_operations ops{};
    ops.getattr = [](auto... args) { return handler(getattr)(args...); };
    ops.readdir = &filesystem::readdir;
    ops.open = &filesystem::open;
    ops.read = &filesystem::read;
    return fuse_main(argc, argv, &ops, nullptr);
}
