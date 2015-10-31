#pragma once

#include <memory>
#include <fuse.h>

class FsEntry {
protected:
    using Stat = struct stat;

public:
    using Ptr = std::shared_ptr<FsEntry>;

    virtual explicit operator Stat() = 0;
    virtual bool isValid() = 0;
};

struct FsEntryInvalid : public FsEntry {
    explicit operator Stat() override {
        Stat stat {};
        return stat;
    }
    bool isValid() override final {
        return false;
    }
};

struct FsEntryValid : public FsEntry {
    bool isValid() override final {
        return true;
    }
};

struct FsEntryDirectory : public FsEntryValid {
    explicit operator Stat() override {
        Stat stat {};
        stat.st_mode = S_IFDIR | 0755;
        stat.st_nlink = 2;
        return stat;
    }
};

struct FsEntryFile : public FsEntryValid {

    FsEntryFile(std::size_t size) : size(size) { }

    explicit operator Stat() override {
        Stat stat {};
        stat.st_mode = S_IFREG | 0444; // TODO take this in ctor
        stat.st_nlink = 1;
        stat.st_size = size;
        return stat;
    }

private:
    std::size_t size;
};
