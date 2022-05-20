##########################################################################################
# Copyright (c) 2022 Logan Barnes - All Rights Reserved
##########################################################################################
# Make sure all dependencies are installed:
# apt install -y git cmake build-essential ninja-build lcov valgrind

set(LTB_PROJECT_DIRECTORY ${CTEST_SCRIPT_DIRECTORY}/..)

set(CTEST_SOURCE_DIRECTORY "${LTB_PROJECT_DIRECTORY}")
set(CTEST_BINARY_DIRECTORY "${LTB_PROJECT_DIRECTORY}/build")

set(CTEST_CMAKE_GENERATOR Ninja)
set(CTEST_BUILD_CONFIGURATION Debug)
set(CTEST_BUILD_OPTIONS "-DLTB_ENABLE_TESTING:BOOL=ON -DLTB_LINK_GCOV:BOOL=ON")

set(WITH_MEMCHECK TRUE)
set(WITH_COVERAGE TRUE)

set(ENV{CXXFLAGS} "--coverage -fprofile-arcs -ftest-coverage -fno-inline")

#######################################################################

# Clear the binary directory
ctest_empty_binary_directory(${CTEST_BINARY_DIRECTORY})

find_program(CTEST_COVERAGE_COMMAND NAMES gcov)
find_program(CTEST_MEMORYCHECK_COMMAND NAMES valgrind)

set(CTEST_MEMORYCHECK_COMMAND_OPTIONS "--gen-suppressions=all")
set(CTEST_MEMORYCHECK_COMMAND_OPTIONS "${CTEST_MEMORYCHECK_COMMAND_OPTIONS} --leak-check=full")
set(CTEST_MEMORYCHECK_COMMAND_OPTIONS "${CTEST_MEMORYCHECK_COMMAND_OPTIONS} --leak-resolution=med")
set(CTEST_MEMORYCHECK_COMMAND_OPTIONS "${CTEST_MEMORYCHECK_COMMAND_OPTIONS} --track-origins=yes")
set(CTEST_MEMORYCHECK_COMMAND_OPTIONS "${CTEST_MEMORYCHECK_COMMAND_OPTIONS} --vgdb=no")

set(CTEST_CONFIGURE_COMMAND "${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE:STRING=${CTEST_BUILD_CONFIGURATION}")
set(CTEST_CONFIGURE_COMMAND "${CTEST_CONFIGURE_COMMAND} ${CTEST_BUILD_OPTIONS}")
set(CTEST_CONFIGURE_COMMAND "${CTEST_CONFIGURE_COMMAND} \"-G${CTEST_CMAKE_GENERATOR}\"")
set(CTEST_CONFIGURE_COMMAND "${CTEST_CONFIGURE_COMMAND} \"${CTEST_SOURCE_DIRECTORY}\"")

ctest_start(Experimental)
ctest_configure()
ctest_build()
ctest_test()
if (WITH_COVERAGE AND CTEST_COVERAGE_COMMAND)
    ctest_coverage()
endif ()
if (WITH_MEMCHECK AND CTEST_MEMORYCHECK_COMMAND)
    ctest_memcheck()
endif ()
