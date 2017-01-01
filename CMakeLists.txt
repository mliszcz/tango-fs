cmake_minimum_required(VERSION 2.8.9)

set(PROJECT_NAME "tango-fs")
set(PROJECT_NAME_TEST "tango-fs.test")

set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

project (${PROJECT_NAME})

find_package(fuse 3.0 REQUIRED)
find_package(tango 8.0 REQUIRED)
find_package(Boost COMPONENTS unit_test_framework REQUIRED)

if(FUSE_FOUND)
if(TANGO_FOUND)
if(Boost_FOUND)

    add_definitions(${FUSE_DEFINITIONS})
    # add_definitions(-DTANGO_CLIENT)
    add_definitions(-std=c++14)
    # add_definitions(${TANGO_CFLAGS})
    # TANGO_CFLAGS: -std=c++11;-D__x86_64__;-D__linux__;-D__OSVERSION__=2

    # linker:
    # -ltango -lomniDynamic4 -lCOS4 -llog4tango -lzmq -lomniORB4 -lomnithread

    include_directories(${FUSE_INCLUDE_DIRS})
    include_directories(${TANGO_INCLUDE_DIRS})
    include_directories(${Boost_INCLUDE_DIRS})
    include_directories(include)

    set(LIBS ${LIBS} ${FUSE_LIBRARIES})
    set(LIBS ${LIBS} ${TANGO_LIBRARIES})
    set(LIBS ${LIBS} ${TANGO_LDFLAGS})

    file(GLOB SOURCES "src/*.cpp")
    add_executable(${PROJECT_NAME} ${SOURCES})

    target_link_libraries(${PROJECT_NAME} "${LIBS}")

    file(GLOB TEST_SOURCES "test/*.cpp")
    add_executable(${PROJECT_NAME_TEST} ${TEST_SOURCES})
    target_link_libraries(${PROJECT_NAME_TEST}
                          "${LIBS}"
                          ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

endif()
endif()
endif()
