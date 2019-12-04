# Time-sensitive Intermittent Computing Meets Legacy Software (TICS)

TICS is a framework that allows for C programs to be executed on intermittent power, harvested from the environment.
TICS consists of multiple components that together make sure that the program that is being executed continues where it left off after a power failure. Additionally, TICS does this in a way that leads to checkpoint times that can be bounded to a reasonable upper limit, making reasoning about checkpoint placement dynamically possible (although this is not explored in the current version).

TICS is intended to be used with the [MSP430FR5969](http://www.ti.com/product/MSP430FR5969) microcontroller, but can be adapted to work with any MSP-based microcontroller that consists of non-volatile main memory.

The main components of TICS are:
* **TICS runtime** for memory logging and checkpoint management
* **TICS compiler backend** (GCC and LLVM) for stack segmentation management
* **TICS source instrumentation** for variable instrumentation

## Project Layout
```bash
├── llvm-tics (modified LLVM compiler)
├── msp430-gcc-tics (modified GCC compiler)
├── tics (TICS runtime)
│   ├── arch
│   │   └── msp430x
│   │       ├── lib
│   │       └── llvm-workaround
│   ├── ext (external libraries)
│   ├── include
│   ├── source-instrumentation
│   │   └── memory-log-instrumentation
│   ├── src
│   └── test
│       └── functional (benchmarks)
└── user-study (user study questions and results)
    ├── user-study-analysis
    ├── user-study-questions
    └── user-study-results

```

## Usage

TICS was **tested on Ubuntu 18.04**

### Building GCC
**Required packages:**
```bash
$ apt-get install make gcc g++ libc-dev flex bison texinfo ncurses-dev zlib1g-dev bash curl
```

**Build commands:**
```bash
$ cd msp430-gcc-tics
$ ./build.sh

```

### Building LLVM
**Required packages:**
```bash
$ apt-get install make gcc cmake python zlib1g-dev
```

**Build commands:**
```bash
$ cd llvm-tics
$ ./build.sh

```

### Configuring toolchains
```bash
$ ./setup.sh
```

### Installing MSP430 Support Files
From Texas Instruments website download and extract the [MSP430 GCC support files](http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSPGCC/latest/exports/msp430-gcc-support-files-1.208.zip).


### Building Source Instrumentation Tool
The source instrumentation tool is located in [tics/source-instrumentation/memory-log-instrumentation](tics/source-instrumentation/memory-log-instrumentation/).
This tool is used to instrument global variable and pointer writes.

**Build commands**:
```bash
$ cd tics/source-instrumentation/memory-log-instrumentation
$ mkdir build
$ cd build
$ cmake ../
$ make
```
Optionally tests can be run using `runtest.sh`.


### Building C Code
**Path setup**
Modify the CMake target configuration file [tics/arch/msp430x/msp430-toolchain-ws.cmake](tics/arch/msp430x/msp430-toolchain-ws.cmake) such that the correct paths are configured for GCC, LLVM, libraries and the support files. This can be done by changing all the paths starting with `/home/user`.

**Build commands:**
```bash
$ cd tics
$ ./configure
$ cd build
$ make
```

### Benchmarks
The benchmarks are located in [tics/test/functional/checkpoint](tics/test/functional/checkpoint).
Building the C code will also generate `.elf` files for the benchmarks.
Transformation of the C code to the instrumented version that includes memory logging is done using the source instrumentation tool describe above (`memlog`).
The benchmarks where uploaded using [Code Composer Studio (CCS)](http://www.ti.com/tool/CCSTUDIO).
Benchmark timings where started after the system initialization (`init()`) and where run until the end of the benchmarks (before result validation and the summary of the run).
The time was measured using hardware breakpoints and the build in cycle counter.

## Paper

To cite the paper where TICS is being introduced please use the following LaTeX bibitem.

```
@inproceedings{kortbeek_asplos2020,
    author = "Vito {Kortbeek} and Kasim Sinan {Yildirim} and Abu {Bakar} and Jacob {Sorber}
    and Josiah {Hester} and Przemys{\l}}aw {Pawe{\l}czak}",
    title = "Time-sensitive Intermittent Computing Meets Legacy Software",
    year = "2020",
    booktitle = "Proc. ASPLOS",
    address = "Lausanne, Switzerland",
    month = mar  # " 16--20,",
    publisher = "ACM"
}
```

### Copyright

Copyright (C) 2019 TU Delft Embedded and Networked Systems Group/Sustainable Systems Laboratory.

MIT Licence. See [License](tics/LICENSE) file for details.
