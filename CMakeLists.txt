cmake_minimum_required(VERSION 3.2.0)

set(PROJECT_MAIN "tango-fs")
set(PROJECT_TEST "tango-fs.test")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")
set(CMAKE_CXX_STANDARD 14)

project (${PROJECT_MAIN})

find_package(fuse 3.0 REQUIRED)
find_package(tango 8.0 REQUIRED)
find_package(GTest)
find_package(GMock)

if(FUSE_FOUND)
if(TANGO_FOUND)

    # required by fuse
    add_definitions(-D_FILE_OFFSET_BITS=64)

    include_directories(${FUSE_INCLUDE_DIRS})
    include_directories(${TANGO_INCLUDE_DIRS})
    include_directories("include")

    set(LIBS ${LIBS} ${FUSE_LIBRARIES})
    set(LIBS ${LIBS} ${TANGO_LIBRARIES})

    file(GLOB_RECURSE SOURCES_MAIN "src/*.cpp")
    add_executable(${PROJECT_MAIN} ${SOURCES_MAIN})
    target_link_libraries(${PROJECT_MAIN} ${LIBS})
    target_compile_options(${PROJECT_MAIN} PRIVATE -O3 -Wall)

    if(GTest_FOUND)
    if(GMock_FOUND)

        file(GLOB_RECURSE SOURCES_TEST "test/*.cpp")
        add_executable(${PROJECT_TEST} ${SOURCES_TEST})
        target_link_libraries(${PROJECT_TEST}
                              -Wl,-Bstatic -lgtest -lgmock -lgmock_main
                              -Wl,-Bdynamic -lpthread
                              ${LIBS}
                              -fprofile-arcs
                              -ftest-coverage)

        target_include_directories(${PROJECT_TEST} PRIVATE
                                   ${GTEST_INCLUDE_DIRS}
                                   ${GMOCK_INCLUDE_DIRS}
                                   "test")

        target_compile_options(${PROJECT_TEST} PRIVATE
                               -g -O0 -Wall -fprofile-arcs -ftest-coverage)

    endif()
    endif()

endif()
endif()
