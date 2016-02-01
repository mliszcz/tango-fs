
# tango-fs

FUSE interface for TANGO Control System.

## Examples

### Mounting a filesystem
```
mkdir mount
./tango-fs ./mount
```

### Unmounting
```
fusermount -u ./mount
```

### Operations
*TODO*

## Building

Required libraries and headers:
```
libfuse-dev libtango8-dev
```

Build process is controlled by CMake. C++14-enabled compiler is required.
Steps:
```
$ mkdir build && cd build
$ cmake -DCMAKE_C_COMPILER=$(which gcc-5) -DCMAKE_CXX_COMPILER=$(which g++-5) ..
$ make all
```

Generated executable is stored in `build/tango-fs`.
