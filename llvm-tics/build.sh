#!/bin/bash

BASE_DIR=$(pwd)

# Clone LLVM and Clang
git clone https://github.com/llvm-mirror/llvm.git
pushd llvm
git reset --hard be2b2c32d38
popd

git clone https://github.com/llvm-mirror/clang.git
pushd clang
git reset --hard 52ed5ec631
popd

# Copy Clang into LLVM
mv clang llvm/tools/

echo "Applying MSP430 ISR patch"
pushd llvm/tools/clang
git apply $BASE_DIR/clang-MSP430-accept-all-interrupt-vector-numbers-3-8.patch
popd

echo "Applying bool patch"
pushd llvm
# Fix build with GCC >= 8.1 failing due to implicit cast no longer supported 
git apply $BASE_DIR/bool.patch
popd

echo "Applying TICS llvm patch"
pushd llvm
git apply $BASE_DIR/tics-asplos-llvm.patch
popd

echo "Applying TICS clang patch"
pushd llvm/tools/clang
git apply $BASE_DIR/tics-asplos-clang.patch
popd

# Create build directory
mkdir -p llvm-tics-build

echo "Generating build files"
pushd llvm-tics-build
cmake -G "Unix Makefiles" \
	-DLLVM_OPTIMIZED_TABLEGEN=1 -DLLVM_TARGETS_TO_BUILD="MSP430;X86" \
	-DCMAKE_BUILD_TYPE=MinSizeRel -DCMAKE_LINKER=/usr/bin/ld.gold \
	-DCMAKE_INSTALL_PREFIX=/opt/llvm-tics \
	../llvm
popd

# Build LLVM
echo "Building LLVM"
pushd llvm-tics-build
make -j$(nproc)
#make ocaml_doc
popd

echo "Installing to directory"
mkdir -p install
pushd llvm-tics-build
make DESTDIR=$BASE_DIR/install install
popd

echo "Done"
