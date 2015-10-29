
#define FUSE_USE_VERSION 30

#include <TangoFuseOperations.hpp>

#include <cstdlib>
#include <string>

int main(int argc, char *argv[])
{
    const std::string tango_host = std::string("TANGO_HOST=") + "172.17.0.1" + ":" + "10000";
    putenv(const_cast<char*>(tango_host.c_str()));

    TangoFuseOperations ops {};
    struct fuse_operations fuseOps = static_cast<fuse_operations>(ops);
    return fuse_main(argc, argv, &fuseOps, nullptr);
}
