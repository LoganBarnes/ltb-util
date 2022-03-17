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
#### LtbUtil::ltb-util ###
##########################
ltb_add_library(
        ltb-util
        ${CMAKE_CURRENT_LIST_DIR}/src/ltb/util/async_task_runner.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/ltb/util/atomic_data.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/ltb/util/blocking_queue.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/ltb/util/comparison_utils.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/ltb/util/container_utils.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/ltb/util/duration.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/ltb/util/enum_bits.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/ltb/util/enum_flags.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/ltb/util/error.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/ltb/util/error_callback.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/ltb/util/file_utils.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/ltb/util/generic_guard.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/ltb/util/hash_utils.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/ltb/util/ignore.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/ltb/util/power_of_2.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/ltb/util/priority_tag.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/ltb/util/result.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/ltb/util/string.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/ltb/util/timer.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/ltb/util/type_string.cpp
        # ${CMAKE_CURRENT_LIST_DIR}/src/ltb/util/uuid.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/ltb/util/variant_utils.cpp
)
add_library(
        LtbUtil::ltb-util
        ALIAS
        ltb-util
)
ltb_target_link_libraries(
        ltb-util
        PUBLIC
        Threads::Threads
        tl::expected
        magic_enum::magic_enum
        PRIVATE
        doctest::doctest
)
ltb_target_include_directories(
        ltb-util
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/external>
)
ltb_target_compile_features(
        ltb-util
        PRIVATE
        cxx_std_17
)
set_target_properties(
        ltb-util
        PROPERTIES
        POSITION_INDEPENDENT_CODE ON
        WINDOWS_EXPORT_ALL_SYMBOLS ON
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
