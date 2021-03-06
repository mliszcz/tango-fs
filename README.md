
[![Travis](https://img.shields.io/travis/mliszcz/tango-fs.svg)](https://travis-ci.org/mliszcz/tango-fs)
[![Coveralls](https://img.shields.io/coveralls/mliszcz/tango-fs.svg)](https://coveralls.io/github/mliszcz/tango-fs)

# tango-fs

This is a proof-of-concept of a FUSE interface for TANGO Control System.
The filesystem may be accessed via commandline utilities (`ls`, `cat`, etc.)
or via GUI tools like file browsers and text editors.

Limitations:

* filesystem is read-only,
* no caching is performed.

Requirements:

* libtango,
* libfuse,
* googletest, googlemock (unit tests only)
* tango and fuse headers (build only),
* C++14-enabled compiler (build only).

See `docker-dev` directory for ready-to-go Fedora and Ubuntu images.

## Howto

* mount the TANGO filesystem

  ```
$ mkdir mnt
$ TANGO_HOST=172.18.0.3:10000 ./tango-fs mnt &
```

* unmount the TANGO filesystem

  ```
$ fusermount -u mnt
```

* get help

  ```
$ ./tango-fs -h
```

* build the filesystem daemon

  * First two steps will spawn a Fedora-based container with all dependencies
  installed. You can also use Ubuntu-based image. You may omit these steps if
  your system is up-to-date and you have both tango and fuse installed.
  * The `make` program creates `tango-fs` (and `tango-fs.test` if googletest is
    available) binary in current working directory.
  * Remember to set TANGO_HOST before mounting the filesystem.

  ```
$ docker build -t tango-fs-dev -f ./docker-dev/Dockerfile.fc25 .
$ ./docker-dev/bash

$ mkdir build && cd build
$ cmake ..
$ make

$ ./tango-ts.test

$ mkdir mnt
$ TANGO_HOST=172.18.0.3:10000 ./tango-fs mnt &
```

### Examples

```
(michal@red) {master} ~/Documents/tango/tango-fs/build $ tree mnt
mnt
├── dserver
│   ├── DataBaseds
│   │   └── 2
│   │       ├── attributes
│   │       │   ├── State
│   │       │   │   └── value
│   │       │   └── Status
│   │       │       └── value
│   │       ├── class
│   │       ├── description
│   │       ├── name
│   │       └── status
...
└── test
    └── rest
        └── 0
            ├── attributes
            │   ├── State
            │   │   └── value
            │   ├── Status
            │   │   └── value
            │   ├── aliveProxies
            │   │   └── value
            │   ├── attributeValueExpirationDelay
            │   │   └── value
            │   ├── cacheEnabled
            │   │   └── value
            │   ├── proxyKeepAliveDelay
            │   │   └── value
            │   └── staticValueExpirationDelay
            │       └── value
            ├── class
            ├── description
            ├── name
            └── status

157 directories, 171 files
```

```
(michal@red) {master} ~/Documents/tango/tango-fs/build $ cat mnt/sys/tg_test/1/attributes/double_scalar/value
Sat Jan  7 00:00:17 2017 (1483743617,745531 sec) : double_scalar (dim_x = 1, dim_y = 0, w_dim_x = 1, w_dim_y = 0, Data quality factor = VALID, Data format = SCALAR, Data type = DevDouble)
Element number [0] = -221.702
Element number [1] = 0
```

```
(michal@red) {master} ~/Documents/tango/tango-fs/build $ grep -ri --include "status" "device is on" mnt/dserver/
mnt/dserver/DataBaseds/2/status:The device is ON
mnt/dserver/Starter/databaseds/status:The device is ON
mnt/dserver/Starter/mtango/status:The device is ON
mnt/dserver/Starter/tangotest/status:The device is ON
mnt/dserver/TangoAccessControl/1/status:The device is ON
mnt/dserver/TangoRestServer/development/status:The device is ON
mnt/dserver/TangoTest/test/status:The device is ON
```
