#!/bin/sh

rm -r build

DIR="build"
if [ -d "$DIR" ]; then
  echo "Build exist..."
else
  mkdir build
fi
cmake -G "CodeLite - Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -Bbuild
