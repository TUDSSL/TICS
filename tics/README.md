# TICS
### Time-sensitive Intermittent Computing Meets Legacy Software

TICS is a framework that allows for programs to be executed on intermittent power harvested from the environment.
TICS consists of multiple components that together make sure that the program that is being executed continues where it left off after a power failure. Additionally TICS does this in a way that leads to checkpoint times that can be bounded to a reasonable upper limit, making reasoning about checkpoint placement dynamically possible (although this is not explored in the current version).

TICS is intended to be used with the [MSP430FR5969](http://www.ti.com/product/MSP430FR5969) microcontroller, but can be adapted to work with any MSP based microcontroller that consists of non-volatile main memory.

The main components of TICS are:
* **TICS runtime** for memory logging and checkpoint management
* **TICS compiler backend** (GCC and LLVM) for stack segmentation management
* **TICS source instrumentation** for variable instrumentation

