##########################################################################################
# Copyright (c) 2022 Logan Barnes - All Rights Reserved
##########################################################################################
cmake_minimum_required(VERSION 3.16)
project(LtbUtil VERSION 1.0.0)

###############
### Options ###
###############
option(LTB_ENABLE_TESTING "Enable LTB Testing" OFF)

if (LTB_ENABLE_TESTING AND NOT BUILD_TESTING)
    include(CTest)
endif ()

#############################
### CMake Package Manager ###
#############################
include(cmake/CPM.cmake)

#########################
### External Packages ###
#########################
include(cmake/LtbAddTarget.cmake)

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

