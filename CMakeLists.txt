cmake_minimum_required(VERSION 3.9.4)
project(drawbot VERSION 0.1.0)
set(CMAKE_VERBOSE_MAKEFILE ON)

file(GLOB source_files ${PROJECT_SOURCE_DIR}/src/*.cpp)

set(CMAKE_CPP_COMPILER clang++)

add_executable(drawbot ${source_files})
target_compile_features(drawbot PUBLIC cxx_std_20)

# add_subdirectory(onitama-tb)

set_property(TARGET drawbot PROPERTY CXX_STANDARD 23)
set_property(TARGET drawbot PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
set(CMAKE_CXX_FLAGS "-march=native")
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELEASE} -g -gcodeview -D_DLL")

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-instr-generate")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-use=${PROJECT_SOURCE_DIR}/drawbot6.profdata")

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
