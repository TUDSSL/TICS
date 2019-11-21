#!/bin/bash

dirname="msp430-gcc-7.3.1.24-source-full"
buildscript="README-build.sh"

# Download the missing MSP430-GCC sources from Texas Instruments
echo "Downloading and extracting GCC sources"
./download-sources.sh

# Build MSP430-GCC
cd $dirname
if [ ! -f $buildscript ]; then
    echo "Build script: $buildscript not found"
    exit 1
fi

# Make a "fast" version of the build script (make -j)
echo "Creating fast version of the build script"
sed "s/make$/make -j$(nproc)/g" ./$buildscript > ./fast-$buildscript

# Make build script executable
echo "Running build script"
chmod +x fast-$buildscript
./fast-$buildscript

