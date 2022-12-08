# A version I use this on because of pi.
cmake_minimum_required(VERSION 3.14)

# Now we can use C++17 as a standard.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Defined to cool down Microsoft gods.
if(WIN32)
    add_compile_options("/std:c++latest")
    add_compile_options("/D _WIN32_WINNT=0x0601")
endif(WIN32)

# Project name has to be smallercaps and hyphen (see vcpkg.json).
project(grpc-twitter)

# To simplify the whole vcpkg process.
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    include("~/Documents/GitHub/vcpkg/scripts/buildsystems/vcpkg.cmake")
endif()
if(UNIX AND NOT DEFINED VCPKG_TARGET_TRIPLET)
    set(VCPKG_TARGET_TRIPLET x64-linux)
endif()
if(WIN32 AND NOT DEFINED VCPKG_TARGET_TRIPLET)
    set(VCPKG_TARGET_TRIPLET x64-windows)
endif()

# Dependencies (should be precompiled by vcpkg.json).
find_package(protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(Gtest CONFIG REQUIRED)

# search for subdirectory 'CMakeLists.txt'.
add_subdirectory(GrpcLib)
add_subdirectory(TweetLib)
add_subdirectory(TweetTest)
add_subdirectory(TweetClient)
add_subdirectory(TweetServer)
