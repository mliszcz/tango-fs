# Finder module for TANGO control system
#
# The following variables will be defined:
# - TANGO_FOUND : was TANGO found?
# - TANGO_INCLUDE_DIRS : TANGO include directory
# - TANGO_LIBRARIES : TANGO shared library (*.so)
# - TANGO_CFLAGS : TANGO compiler flags
# - TANGO_LDFLAGS : TANGO linker flags
# - TANGO_VERSION : complete version of TANGO (major.minor.patch)
# - TANGO_VERSION_MAJOR : major version of TANGO
# - TANGO_VERSION_MINOR : minor version of TANGO
# - TANGO_VERSION_PATCH : minor version of TANGO

#==============================================================================
# The MIT License (MIT)
#
# Copyright (c) 2015 Michal Liszcz
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#==============================================================================

cmake_minimum_required(VERSION 2.8.9)

set(DEBUG_ON TRUE)

function(debug _varname)
    if(DEBUG_ON)
        message("${_varname} = ${${_varname}}")
    endif(DEBUG_ON)
endfunction(debug)

set(TANGO_FOUND TRUE)
set(TANGO_INCLUDE_DIRS )
set(TANGO_LIBRARIES )
set(TANGO_CFLAGS )
set(TANGO_LDFLAGS )

find_package(PkgConfig)

set(PC_TANGO_INCLUDE_DIRS )
set(PC_TANGO_LIBRARY_DIRS )

if(PKG_CONFIG_FOUND)
    pkg_check_modules(PC_TANGO "tango" QUIET)
    if(PC_TANGO_FOUND)

        debug(PC_TANGO_LIBRARIES)
        debug(PC_TANGO_LIBRARY_DIRS)
        debug(PC_TANGO_LDFLAGS)
        debug(PC_TANGO_LDFLAGS_OTHER)
        debug(PC_TANGO_INCLUDE_DIRS)
        debug(PC_TANGO_CFLAGS)
        debug(PC_TANGO_CFLAGS_OTHER)

        set(TANGO_CFLAGS "${PC_TANGO_CFLAGS_OTHER}")
        set(TANGO_LDFLAGS "${PC_TANGO_LDFLAGS}")

    endif(PC_TANGO_FOUND)
endif(PKG_CONFIG_FOUND)

find_path(
    TANGO_INCLUDE_DIRS
    NAMES "tango.h" "tango_const.h"
    PATHS "${PC_TANGO_INCLUDE_DIRS}" "/usr/include/tango"
    DOC "Include directories for TANGO"
)

if(NOT TANGO_INCLUDE_DIRS)
    set(TANGO_FOUND FALSE)
endif(NOT TANGO_INCLUDE_DIRS)

find_library(
    TANGO_LIBRARIES
    NAMES "tango"
    PATHS "${PC_TANGO_LIBRARY_DIRS}"
    DOC "Libraries for TANGO"
)

if(NOT TANGO_LIBRARIES)
    set(TANGO_FOUND FALSE)
endif(NOT TANGO_LIBRARIES)

if(TANGO_FOUND)
    if(EXISTS "${TANGO_INCLUDE_DIRS}/tango_const.h")
        file(READ "${TANGO_INCLUDE_DIRS}/tango_const.h" _contents)
        string(REGEX REPLACE ".*# *define *TANGO_VERSION_MAJOR *([0-9]+).*" "\\1" TANGO_VERSION_MAJOR "${_contents}")
        string(REGEX REPLACE ".*# *define *TANGO_VERSION_MINOR *([0-9]+).*" "\\1" TANGO_VERSION_MINOR "${_contents}")
        string(REGEX REPLACE ".*# *define *TANGO_VERSION_PATCH *([0-9]+).*" "\\1" TANGO_VERSION_PATCH "${_contents}")
        set(TANGO_VERSION "${TANGO_VERSION_MAJOR}.${TANGO_VERSION_MINOR}.${TANGO_VERSION_PATCH}")
    endif()
endif()

debug(TANGO_FOUND)
debug(TANGO_INCLUDE_DIRS)
debug(TANGO_LIBRARIES)
