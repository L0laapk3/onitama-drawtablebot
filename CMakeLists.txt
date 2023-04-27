cmake_minimum_required(VERSION 3.9.4)
project(drawbot VERSION 0.1.0)
set(CMAKE_VERBOSE_MAKEFILE ON)

file(GLOB_RECURSE source_files ${PROJECT_SOURCE_DIR}/src/**.cpp)

set(CMAKE_CPP_COMPILER clang++)

add_executable(drawbot ${source_files})
target_compile_features(drawbot PUBLIC cxx_std_20)
target_include_directories(drawbot PUBLIC ${PROJECT_SOURCE_DIR}/src)


## DOCTEST
include(ExternalProject)
find_package(Git REQUIRED)

ExternalProject_Add(
    doctest
    PREFIX ${CMAKE_BINARY_DIR}/doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    TIMEOUT 10
    UPDATE_COMMAND ${GIT_EXECUTABLE} pull
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
)

# Expose required variable (DOCTEST_INCLUDE_DIR) to parent scope
ExternalProject_Get_Property(doctest source_dir)
set(DOCTEST_INCLUDE_DIR ${source_dir}/doctest CACHE INTERNAL "Path to include folder for doctest")

include_directories(${DOCTEST_INCLUDE_DIR})
## /DOCTEST



# add_subdirectory(onitama-tb)

set_property(TARGET drawbot PROPERTY CXX_STANDARD 23)
set_property(TARGET drawbot PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
set(CMAKE_CXX_FLAGS "-march=native")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELEASE} -g -D_DLL")

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-instr-generate")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-use=${PROJECT_SOURCE_DIR}/drawbot6.profdata")

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
