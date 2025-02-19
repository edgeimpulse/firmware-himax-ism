# Edge Impulse firmware for Himax ISM Development Kit

[Edge Impulse](https://www.edgeimpulse.com) enables developers to create the next generation of intelligent device solutions with embedded Machine Learning. This repository contains the Edge Impulse firmware for the Himax ISM Development Kit. This device supports all Edge Impulse device features, including ingestion and inferencing.

> **Note:**  Do you just want to use this development board with Edge Impulse? No need to build this firmware. See the instructions [here](https://docs.edgeimpulse.com/docs/edge-ai-hardware/mcu-+-ai-accelerators/himax-ism-wise-eye-2) for a prebuilt image and instructions.

## Development

The source code fo the Edge Impulse firmware can be found here: WE2_CM55M_APP_S/app/scenario_app/edge_impulse_firmware

## Building firmware (native)

1. Install required tools

* [Git](https://git-scm.com/downloads) - make sure `git` is in your PATH.
* [GNU ARM Embedded Toolchain 13.2.rel1](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads/13-2-rel1)

2. Clone this repository
3. In the repository root, run the build script
   ```
   ./build.sh --build
   ```
4. Your firmware should be ready in the `firmware.img` file in the root directory.

## Building firmware (Docker)

1. Install required tools

* [Docker Desktop](https://www.docker.com/products/docker-desktop/)

2. Clone this repository
3. Build Docker image (in the repository root)
   ```
   docker build -t himax-we2 .
   ```
4. Buld the firmware
   ```
   docker run --rm -v $PWD:/app himax-we2 ./build.sh --build
   ```
5. Your firmware should be ready in the `firmware.img` file in the root directory.

## Flashing the firmware

> **Note:** Flashing is currently supported only under Windows x86 operating system. For more details, reach Himax support.

1. Clone [this repository](https://github.com/HimaxWiseEyePlus/Himax-WiseEye-Module-G1-SDK)
2. Follow the instructions in [this document](https://github.com/HimaxWiseEyePlus/Himax-WiseEye-Module-G1-SDK/blob/main/_Documents/2_EVK_and_PC_Tool_User_Guide_HX6538_ISM028_03M_V1.1.pdf)
