##########################################################################################
# Copyright (c) 2022 Logan Barnes - All Rights Reserved
##########################################################################################
cmake_minimum_required(VERSION 3.21)
project(LtbUtil VERSION 1.0.0)

###############
### Options ###
###############
option(LTB_ENABLE_TESTING "Enable LTB Testing" OFF)

#############################
### CMake Package Manager ###
#############################
set(CPM_DOWNLOAD_VERSION 0.35.0)

if (CPM_SOURCE_CACHE)
    set(CPM_DOWNLOAD_LOCATION "${CPM_SOURCE_CACHE}/cpm/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
elseif (DEFINED ENV{CPM_SOURCE_CACHE})
    set(CPM_DOWNLOAD_LOCATION "$ENV{CPM_SOURCE_CACHE}/cpm/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
else ()
    set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
endif ()

if (NOT (EXISTS ${CPM_DOWNLOAD_LOCATION}))
    message(STATUS "Downloading CPM.cmake to ${CPM_DOWNLOAD_LOCATION}")
    file(DOWNLOAD
         https://github.com/TheLartians/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake
         ${CPM_DOWNLOAD_LOCATION}
         )
endif ()

include(${CPM_DOWNLOAD_LOCATION})

#########################
### External Packages ###
#########################
include(${CMAKE_CURRENT_LIST_DIR}/cmake/LtbAddTarget.cmake)

find_package(Threads REQUIRED)

CPMAddPackage("gh:doctest/doctest@2.4.8")
CPMAddPackage(
        NAME expected
        GIT_TAG 96d547c03d2feab8db64c53c3744a9b4a7c8f2c5 # master
        GITHUB_REPOSITORY TartanLlama/expected
        OPTIONS "EXPECTED_BUILD_TESTS OFF"
)
CPMAddPackage("gh:Neargye/magic_enum@0.7.3")
CPMAddPackage("gh:TheLartians/PackageProject.cmake@1.6.0")

##########################
#### LtbUtil::LtbUtil ###
##########################
ltb_add_library(
        LtbUtil
        src/async_task_runner.cpp
        src/atomic_data.cpp
        src/blocking_queue.cpp
        src/comparison_utils.cpp
        src/container_utils.cpp
        src/duration.cpp
        src/enum_bits.cpp
        src/enum_flags.cpp
        src/error.cpp
        src/error_callback.cpp
        src/file_utils.cpp
        src/generic_guard.cpp
        src/hash_utils.cpp
        src/ignore.cpp
        src/power_of_2.cpp
        src/priority_tag.cpp
        src/result.cpp
        src/string.cpp
        src/timer.cpp
        src/type_string.cpp
        # src/uuid.cpp
        src/variant_utils.cpp
)
add_library(
        LtbUtil::LtbUtil
        ALIAS
        LtbUtil
)
ltb_target_link_libraries(
        LtbUtil
        PUBLIC
        Threads::Threads
        tl::expected
        magic_enum::magic_enum
        PRIVATE
        doctest::doctest
)
ltb_target_include_directories(
        LtbUtil
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
        $<INSTALL_INTERFACE:include>
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src>
)
ltb_target_compile_features(
        LtbUtil
        PRIVATE
        cxx_std_17
)
set_target_properties(
        LtbUtil
        PROPERTIES
        POSITION_INDEPENDENT_CODE ON
        WINDOWS_EXPORT_ALL_SYMBOLS ON
)
target_compile_options(
        LtbUtil
        PRIVATE
        $<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/experimental:newLambdaProcessor>
)

#################
### Packaging ###
#################
#packageProject(
#        # the name of the target to export
#        NAME ${PROJECT_NAME}
#        # the version of the target to export
#        VERSION ${PROJECT_VERSION}
#        # a temporary directory to create the config files
#        BINARY_DIR ${PROJECT_BINARY_DIR}
#        # location of the target's public headers
#        INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include
#        # should match the target's INSTALL_INTERFACE include directory
#        INCLUDE_DESTINATION include/${PROJECT_NAME}-${PROJECT_VERSION}
#        # semicolon separated list of the project's dependencies
#        DEPENDENCIES "fmt 7.1.3;cxxopts 2.2.0"
#        # (optional) create a header containing the version info
#        # Note: that the path to headers should be lowercase
#        VERSION_HEADER "${PROJECT_NAME}/version.h"
#        # (optional) create a export header using GenerateExportHeader module
#        EXPORT_HEADER "${PROJECT_NAME}/export.h"
#        # (optional) install your library with a namespace (Note: do NOT add extra '::')
#        NAMESPACE ${PROJECT_NAMESPACE}
#        # (optional) define the project's version compatibility, defaults to `AnyNewerVersion`
#        # supported values: `AnyNewerVersion|SameMajorVersion|SameMinorVersion|ExactVersion`
#        COMPATIBILITY AnyNewerVersion
#        # (optional) option to disable the versioning of install destinations
#        DISABLE_VERSION_SUFFIX YES
#        # (optional) option to ignore target architecture for package resolution
#        # defaults to YES for header only (i.e. INTERFACE) libraries
#        ARCH_INDEPENDENT YES
#)
