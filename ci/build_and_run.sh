#!/usr/bin/env bash

set -e # fail script if any individual commands fail

function build_and_run() {
  cmake -E make_directory "${1}"
  # Compile without tests enabled first
  cmake -E chdir "${1}" cmake .. -DCMAKE_BUILD_TYPE="${2}" -DLTB_USE_DEV_FLAGS=ON -DLTB_BUILD_TESTS=OFF
  cmake -E chdir "${1}" cmake --build . --parallel
  # Enable tests and rebuild
  cmake -E chdir "${1}" cmake .. -DLTB_BUILD_TESTS=ON
  cmake -E chdir "${1}" cmake --build . --parallel
  # Run the tests
  cmake -E chdir "${1}" ctest
}

build_and_run docker-cmake-build-debug Debug
build_and_run docker-cmake-build-release Release

# Generate coverage reports
COVERAGE_TARGETS="$(find docker-cmake-build-debug -maxdepth 1 -executable -type f -name 'test_*' -exec basename {} \; | sed 's/[^ ]* */&_coverage/g')"
# shellcheck disable=SC2086
cmake -E chdir docker-cmake-build-debug cmake --build . --target ${COVERAGE_TARGETS}
