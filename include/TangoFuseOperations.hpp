#pragma once

#include <FuseOperations.hpp>

#include <cstring>
#include <tango.h>
#include <errno.h>
#include <fuse.h>

class TangoFuseOperations : public FuseOperations {

private:
    const char* hello_str;
    const char* hello_path;

public:

    TangoFuseOperations() :
        hello_str("Hello World!\n"),
        hello_path("/hello") { }

    int getattr(const char* path,
                struct stat* stbuf) override {

        // auto database = std::make_shared<Tango::Database>();
        // std::vector<std::string> domains;
        //
        // std::string dd_pattern("dserver/TangoTest/test*");
        // DbDatum dd = database->get_device_exported(dd_pattern);
        // dd >> domains;
        //
        // for (auto& d : domains) {
        //     std::cout << "domain " << d << "\n";
        // }

        int res = 0;
        memset(stbuf, 0, sizeof(struct stat));
        if (strcmp(path, "/") == 0) {
                stbuf->st_mode = S_IFDIR | 0755;
                stbuf->st_nlink = 2;
        } else if (strcmp(path, hello_path) == 0) {
                stbuf->st_mode = S_IFREG | 0444;
                stbuf->st_nlink = 1;
                stbuf->st_size = strlen(hello_str);
        } else
                res = -ENOENT;
        return res;
    }

    int readdir(const char* path,
                void* buf,
                fuse_fill_dir_t filler,
                off_t offset,
                struct fuse_file_info* fi) override {
        (void) offset;
        (void) fi;
        if (strcmp(path, "/") != 0)
                return -ENOENT;
        filler(buf, ".", NULL, 0);
        filler(buf, "..", NULL, 0);
        filler(buf, hello_path + 1, NULL, 0);
        return 0;
    }

    int open(const char* path,
             struct fuse_file_info* fi) override {
        if (strcmp(path, hello_path) != 0)
                return -ENOENT;
        if ((fi->flags & 3) != O_RDONLY)
                return -EACCES;
        return 0;
    }

    int read(const char* path,
             char* buf,
             size_t size,
             off_t offset,
             struct fuse_file_info* fi) override {
        size_t len;
        (void) fi;
        if(strcmp(path, hello_path) != 0)
                return -ENOENT;
        len = strlen(hello_str);
        if (offset < len) {
                if (offset + size > len)
                        size = len - offset;
                memcpy(buf, hello_str + offset, size);
        } else
                size = 0;
        return size;
    }
};
