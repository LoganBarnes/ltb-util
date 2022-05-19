#!/usr/bin/env sh
SRC_FILE_LIST="$(find src -type f -name '*.cpp' -o -name '*.hpp' -o -name '*.cu' -o -name '*.cuh')"
INC_FILE_LIST="$(find include -type f -name '*.hpp' -o -name '*.cuh' | grep -v google)"
#echo "Formatting: ${SRC_FILE_LIST}"
#echo "Formatting: ${INC_FILE_LIST}"
clang-format-14 -i -style=file ${SRC_FILE_LIST}
clang-format-14 -i -style=file ${INC_FILE_LIST}
