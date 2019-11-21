#!/bin/bash

MSP430_GCC_OBJ_ROOT=/home/tics/ASPLOS2020/TICS/msp430-gcc-tics/msp430-gcc-7.3.1.24-source-full/install/usr/local
MSP430_LLVM_OBJ_ROOT=/home/tics/ASPLOS2020/TICS/llvm-tics/install/opt/llvm-tics

# Setup paths
export PATH=$MSP430_GCC_OBJ_ROOT/bin:$MSP430_LLVM_OBJ_ROOT/bin:$PATH
