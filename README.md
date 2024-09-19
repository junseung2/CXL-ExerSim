# CXL-ExerSim (Compute eXpress Link Exercise and Simulator)

### Author

**Junseung Lee**

- ljs2021@g.skku.edu

<aside>
💡 CXL-ExerSim aims to understand the detailed actions performed by each layer of the CXL controller and the behavior of the CXL switch. It is C++ based, and all behavior is based on the CXL spec.


</aside>

# What you get with CXL-ExerSim…

## 01_Transaction Layer

- Understand the concept of cache coherence.
- Understand the interaction between Host and Device over the CXL.cache/CXL.mem protocol.

## 02_Link Layer

- Understand the flit packing rules for creating valid flits.
- Understand the retry mechanism of CXL.
- Understand CRC generation and checks.

## 03_ARB/MUX Layer

- Understand dynamic muxing for different flit domains.
- Understand how vLSM controls the state of the link.

## 04_Logical PHY Layer

- Understand how protocol IDs are generated and flits are sent over CXL link.

## 05_CXL Switch

- Understand how the CXL Switch works and is controlled.


| TABLE I: System configurations.             |
|---------------------------------------------|
| **Dual-socket server system**               |
| **Component** | **Description**             |
|--------------|------------------------------|
| OS (kernel)  | Ubuntu 22.04.2 LTS (Linux kernel v6.2) |
| CPU          | 2x AMD® EPYC 9124 CPUs @3.0 GHz, 16 cores |
| Memory       | 4x 32GB DDR5-4800 channels    |
|              | 1x 2TB NVMe M.2 SSD           |
| FPGA         | AMD(Xilinx) Versal HBM Series VHK 158 |
| PCIe         | Gen 5                         |
