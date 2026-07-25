# SimpleSSD Installation and Usage Guide

## Table of Contents

- [SimpleSSD Installation](#simplessd-installation)
  - [1. Version Information](#1-version-information)
  - [2. Virtual Machine Installation](#2-virtual-machine-installation)
  - [3. VM Environment Configuration](#3-vm-environment-configuration)
  - [4. Build SimpleSSD](#4-build-simplessd)
- [SimpleSSD Usage](#simplessd-usage)
  - [1. Preprocess Trace](#1-preprocess-trace)
  - [2. Compile and Run](#2-compile-and-run)

---

## SimpleSSD Installation

### 1. Version Information

> **Recommended versions:**

| Software | Version | Download Link |
|----------|---------|---------------|
| Ubuntu   | 20.04.6 | https://releases.ubuntu.com/20.04/ |

---

### 2. Virtual Machine Installation

Install Ubuntu 20.04 on VMware Workstation (or any other virtualization platform). Follow any standard tutorial online.

**Key steps:**
- Create a new VM with at least 8 cores and 16 GB RAM (more is better).
- Allocate at least 100 GB of disk space.

---

### 3. VM Environment Configuration

Update package lists and install all required dependencies:

```bash
# Update package index
sudo apt update

# For Debian/Ubuntu based Linux
sudo apt install build-essential
sudo apt install scons python-dev zlib1g-dev m4 cmake
sudo apt install libprotobuf-dev protobuf-compiler
sudo apt install libgoogle-perftools-dev
sudo apt install git
```

---

### 4. Build SimpleSSD

Inside the source directory, configure with CMake and build:

```bash
# Configure (Debug build off)
cmake -DDEBUG_BUILD=off .

# Build with 8 parallel jobs
make -j 8
```

> **Tip:** Adjust `-j 8` to match the number of CPU cores in your VM for faster builds.

---

## SimpleSSD Usage

### 1. Preprocess Trace

Prepare your workload trace into the required format. The trace file should follow this structure:

| Timestamp | Operation | Logical Block Address (LBA) | Size (blocks) |
|-----------|-----------|-----------------------------|---------------|
| 0.000018096 | WS | 1540144 | 8 |
| 0.000022009 | WS | 1563344 | 40 |
| 0.540622983 | WS | 1540128 | 8 |
| 0.540631430 | WS | 27952 | 8 |
| 0.540634516 | WS | 27960 | 8 |
| 0.550106507 | WS | 27928 | 8 |

**Naming convention:**  
Name your preprocessed trace file as `test1.revised`, `test2.revised`, etc., and place it inside the `trace/` directory of the project.

---

### 2. Compile and Run

Navigate to the project directory and execute:

```bash
# Build the project
make

# Run the simulation
bash run_sub.sh
```

**Important notes:**
- Ensure the `make` step completes **without any errors** before running the script.
- The `run_sub.sh` script contains key configuration parameters (e.g., SSD size, trace path, output settings). Review and modify them as needed.
- All output data will be generated under the `data/` folder after the simulation finishes.

---


## References

- SimpleSSD Official Repository: https://github.com/simplessd/simplessd-standalone
- Ubuntu 20.04 Download: https://releases.ubuntu.com/20.04/
- GitHub SSH Key Setup: https://docs.github.com/en/authentication/connecting-to-github-with-ssh

---

## License

This guide is provided for research and educational purposes. Please refer to the SimpleSSD project license for code usage terms.
