## Introduction
This is a tool set for SampledFootprint project. It includes the fucntions:
- Getting full memory access trace
- Doing randomly sampling on a trace
- Calculating sampled footprint

## Prerequisite
### Library
- cmake
- boost_program_options
- lz4
### Tool
- Intel Pin

## Install
```sh
mkdir build
cd build
cmake ..
make -j
```

The binary will be in build/src

## Tool
**trace_pin/obj-intel64/trace.so** Pin tool getting the full trace, the trace is filtered with a simulated fully associative LRU cache, then will be compressed using lz4 algorithm.

**RandomSampleWSS** Randomly sample a trace and calculating the sampled working set size with fixed window size. The output will be a file of working set sizes, compressed with lz4.

**RandomSampleWSSResult** Similar with **RandomSampleWSS**, the output will be the sampled footprint and will be printed on the screen.

**USampleWSS** Similar with **RandomSampleWSS**, execpt uniform sampling is used.

**plot/getFP** Get footprint of a trace

**plot/histWSS** input will be a file of sampled working set sizes, such as the output of **RandomSampleWSS**.

