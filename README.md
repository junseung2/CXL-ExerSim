# CXL-ExerSim

**Department:** Sungkyunkwan University  
**Author:** Junseung Lee (junseung0728@naver.com)

This simulator is based on C++ to make it easier to understand the behavior of the CXL platform. All behavior is implemented based on the CXL spec 2.0. If you see anything wrong with simulator, feel free to email me.

## Version 1.0
- Support high-level CXL-based architecture platform
- Supporting simple CXL.io, CXL.cache and CXL.mem protocol
- Supporting CXL Type 1, 2, 3

### Schematic of the CXL Type 2 platform as implemented in CXL-Exersim.
<img width="694" alt="CXL-platform" src="https://github.com/junseung2/CXL-ExerSim/assets/105153659/cf69c30d-fbad-416b-959f-ce3376da8a2f">

CXL-ExerSim version 1 focuses on the transaction flow in the following scenarios for each CXL Type. 
1. When a host accesses(memory read/write) host local memory. 
2. When a host accessing device memory.
3. When a device accesses device memory.
4. When a device accesses host local memory.

It also supports bias mode for CXL Type2 platform.  

### Implemented Part
Host Part 
- Simple Processor Cores
- Host Cache & Memory 
- Host Remote Snoop Filter (HRSF)
- IOMMU

Device Part
- Simple Device Core
- DMA Engine
- Device Remote Snoop Filter (DRSF)
- Device Local Snoop Filter (DLSF)
- Device Cache & Memory
- Transition Agent (TA)
- Bias Table



# Future Work. Coming Soon!

## Version 2.0

- Support detailed CXL.io, CXL.cache, CXL.mem protocol in CXL-based architecture platform
- Support for cache coherency mechanisms (MESI)


## Version 3.0

- Add Switches in CXL-based architecture platform
