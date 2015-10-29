#pragma once

#include <fuse.h>
#include <invoker.hpp>

/**
 * @see http://fuse.sourceforge.net/doxygen/structfuse__operations.html
 */
class FuseOperations {

    template <typename... Args>
    using FuseCallback = invoker::BoundFnPtr<FuseOperations, int, Args...>;

private:

    FuseCallback<const char*,
                 struct stat*> fnGetattr;

    FuseCallback<const char*,
                 void*,
                 fuse_fill_dir_t,
                 off_t,
                 struct fuse_file_info*> fnReaddir;

    FuseCallback<const char*,
                 struct fuse_file_info*> fnOpen;

    FuseCallback<const char*,
                 char*,
                 size_t,
                 off_t,
                 struct fuse_file_info*> fnRead;

protected:

    static const int ENOIMPL = 1;

    FuseOperations() :
        fnGetattr(this, &FuseOperations::getattr),
        fnReaddir(this, &FuseOperations::readdir),
        fnOpen(this, &FuseOperations::open),
        fnRead(this, &FuseOperations::read) { }

public:

    explicit operator fuse_operations() {
        struct fuse_operations ops {};
        ops.getattr = fnGetattr;
        ops.readdir = fnReaddir;
        ops.open = fnOpen;
        ops.read = fnRead;
        return ops;
    }

    virtual int getattr(const char* path,
                        struct stat* stbuf) {
        return -ENOIMPL;
    }

    virtual int readdir(const char* path,
                        void* buf,
                        fuse_fill_dir_t filler,
                        off_t offset,
                        struct fuse_file_info* fi) {
        return -ENOIMPL;
    }

    virtual int open(const char* path,
                     struct fuse_file_info* fi) {
        return -ENOIMPL;
    }

    virtual int read(const char* path,
                     char* buf,
                     size_t size,
                     off_t offset,
                     struct fuse_file_info* fi) {
        return -ENOIMPL;
    }
};
