# MSP430-GCC-OPENSOURCE GCC

The master branch of this repository contains `gcc` directory of the msp430-gcc
opensource compiler toolchain from Texas Instruments [1], developed by Mitto
Systems. The tarball containing the full source is called
`msp430-gcc-7.3.1.24-source-full.tar.bz2
`
and can be downloaded from the Texas Instruments website [2].
Only the `gcc` directory is present here as the extracted tarball including all
the git information would total around 2.3GB, exceeding the recomended maximum
of a github repository by 1.3GB.

This repository is in no way affiliated with Texas Instruments, Mitto Systems,
or any other entity working on the development of the msp430-gcc toolchain. It
merely exist to make it easier to work on/with the compiler sources.

[1] http://www.ti.com/tool/MSP430-GCC-OPENSOURCE  
[2] http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSPGCC/latest/index_FDS.html

## Downloading the remaining source files
To build the complete toolchain the remaining source files need to be
downloaded. This can be done by running `download-sources.sh`.
This script download the `msp430-gcc-7.3.1.24-source-full.tar.bz2` tarball from
Texas Instruments, extracts it, and replaces the `gcc` directory with the one in
this repository.

## Building
Texas Instruments provides a `README-build.sh`. This shell script can be
executed to build the complete toolchain. This script can also be used to
extract the commands needed to build individual components of the toolchain.

The required packages for building are :
```
make gcc g++ libc-dev flex bison texinfo ncurses-dev zlib1g-dev bash curl
```
