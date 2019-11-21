#!/bin/bash

# Build both the LLVM and GCC TICS compiler

echo "Building the TICS GCC compiler"
pushd msp430-gcc-tics
./build.sh
popd

echo "Building the TICS LVM compiler"
pushd llvm-tics
./build.sh
popd

