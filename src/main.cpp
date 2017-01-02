
#define FUSE_USE_VERSION 30

#include <filesystem.hpp>

int main(int argc, char *argv[])
{
    struct fuse_operations ops{};
    ops.getattr = &filesystem::getattr;
    ops.readdir = &filesystem::readdir;
    ops.open = &filesystem::open;
    ops.read = &filesystem::read;
    return fuse_main(argc, argv, &ops, nullptr);
}
