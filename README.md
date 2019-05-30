# BFS_MetaCL
Description

Ported Breadth First Search from https://github.com/pathscale/rodinia/tree/master/opencl/bfs using MetaCL

MetaCL and MetaMorph:

MetaMorph (https://github.com/vtsynergy/MetaMorph) is a framework designed to effectively utilize HPC systems that consist of multiple heterogeneous nodes with different hardware accelerators. It acts as middleware between the application code and compute devices, such as CPUs, GPUs, Intel MIC and FPGAs. MetaMorph hides the complexity of developing code for and executing on heterogeneous platform by acting as a unified “meta-platform.” Metamorph is developed by the Synergy Laboratory @ Virginia Tech (http://synergy.cs.vt.edu/)

MetaCL is a tool that takes OpenCL kernels as input and generates host-side wrappers. These wrappers effectively make OpenCL host-side code developement efficient. MetaCL makes use of Metamorph's OpenCL APIs and backend for generating host-side wrappers. 

Metacl Tutorial: https://github.com/vtsynergy/MetaMorph/tree/c661366a13dec9eb7b9876c91695da48e96d8ba8/metamorph-generators/opencl/docs/tutorials
