# ##############################################################################
# Copyright (c) 2022 Logan Barnes - All Rights Reserved
# ##############################################################################
cmake_minimum_required(VERSION 3.16)
project(LtbUtil VERSION 1.0.0)

# ##############################################################################
# Options
# ##############################################################################
option(LTB_ENABLE_TESTING "Enable LTB Testing" OFF)

if(LTB_ENABLE_TESTING AND NOT BUILD_TESTING)
    include(CTest)
endif()

# ##############################################################################
# CMake Package Manager
# ##############################################################################
include(cmake/CPM.cmake)

# ##############################################################################
# External Packages
# ##############################################################################
include(cmake/LtbCreateDefaultTargets.cmake)

find_package(Threads REQUIRED)

cpmaddpackage("gh:doctest/doctest@2.4.8")
cpmaddpackage(NAME
              expected
              GIT_TAG
              96d547c03d2feab8db64c53c3744a9b4a7c8f2c5 # master
              GITHUB_REPOSITORY
              TartanLlama/expected
              OPTIONS
              "EXPECTED_BUILD_TESTS OFF"
              )
cpmaddpackage("gh:Neargye/magic_enum@0.7.3")

if(doctest_ADDED)
    target_include_directories(doctest SYSTEM
                               INTERFACE
                                   $<BUILD_INTERFACE:${doctest_SOURCE_DIR}>
                               )
endif()

# ##############################################################################
# LtbUtil::LtbUtil
# ##############################################################################
ltb_create_default_targets(LtbUtil
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

# Public
target_link_libraries(LtbUtil_deps INTERFACE Threads::Threads tl::expected
                                             magic_enum::magic_enum
                      )
target_include_directories(LtbUtil_deps
                           INTERFACE
                               $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
                           )

# Private
target_link_libraries(LtbUtil_objs PRIVATE doctest::doctest)
target_include_directories(LtbUtil_objs
                           PRIVATE
                               $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src>
                           )
target_compile_options(LtbUtil_objs
                       PRIVATE
                           $<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/experimental:newLambdaProcessor>
                       )

# Testing
if(TARGET test_LtbUtil)
    target_link_libraries(test_LtbUtil PRIVATE doctest_with_main)
endif()
