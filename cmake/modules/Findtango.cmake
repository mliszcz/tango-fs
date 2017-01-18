
# Finder module for TANGO control system
#
# The following variables will be defined:
# - TANGO_FOUND        : was TANGO found?
# - TANGO_INCLUDE_DIRS : TANGO include directory
# - TANGO_LIBRARIES    : TANGO library and dependencies
# - PC_TANGO_LDFLAGS   : TANGO raw linker flags (as provided by PkgConfig)

find_package(PkgConfig REQUIRED)
include(FindPackageHandleStandardArgs)

set(TANGO_DEBUG_ON FALSE)

function(debug _varname)
    if(TANGO_DEBUG_ON)
        message("${_varname} = ${${_varname}}")
    endif()
endfunction()

pkg_check_modules(PC_TANGO "tango" QUIET)
if(PC_TANGO_FOUND)
    debug(PC_TANGO_LIBRARIES)
    debug(PC_TANGO_LIBRARY_DIRS)
    debug(PC_TANGO_LDFLAGS)
    debug(PC_TANGO_LDFLAGS_OTHER)
    debug(PC_TANGO_INCLUDE_DIRS)
    debug(PC_TANGO_CFLAGS)
    debug(PC_TANGO_CFLAGS_OTHER)
endif()

find_path(
    TANGO_INCLUDE_DIRS
    NAMES "tango.h" "tango_const.h"
    PATHS ${PC_TANGO_INCLUDE_DIRS} "/usr/local/include"
    PATH_SUFFIXES "tango")

foreach(LIB_NAME ${PC_TANGO_LIBRARIES})
    unset(LIB CACHE)
    find_library(
        LIB
        NAMES ${LIB_NAME}
        PATHS ${PC_TANGO_LIBRARY_DIRS} "/usr/local/lib64" "/usr/local/lib")
    list(APPEND TANGO_LIBRARIES ${LIB})
endforeach()

find_package_handle_standard_args(
    TANGO
    DEFAULT_MSG
    TANGO_INCLUDE_DIRS
    TANGO_LIBRARIES
    PC_TANGO_FOUND)

mark_as_advanced(
    TANGO_INCLUDE_DIRS
    TANGO_LIBRARIES)

debug(TANGO_FOUND)
debug(TANGO_INCLUDE_DIRS)
debug(TANGO_LIBRARIES)
