#!/bin/bash

# Clean all build files etc
git clean -fdx

# Remove LLVM repository
rm -rf llvm-tics/llvm
