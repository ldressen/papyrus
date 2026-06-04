#!/usr/bin/env bash

set -e
BUILD_TYPE="Debug"
RUN_AFTER_BUILD=false

for arg in "$@"; do
case "$arg" in
-r)
BUILD_TYPE="Release"
;;
-run)
RUN_AFTER_BUILD=true
;;
*)
echo "Unknown argument: $arg"
exit 1
;;
esac
done

cmake -B build -DCMAKE_BUILD_TYPE=$BUILD_TYPE
cmake --build build -j$(nproc)

if [ "$RUN_AFTER_BUILD" = true ]; then
./build/papyrus
fi
