
#pragma once

#include <fuse.h>

namespace handlers {

// TODO: check if more robust implementation is required here,
// ie. is it possible to get open for a directory or for a path
// where getattr returned -ENOENT

template <typename Path>
auto open(const Path&) {
    return [](auto...) {
        return [](auto&&...) {
            return 0;
        };
    };
}

} // namespace handlers
