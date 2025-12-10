FROM ubuntu:22.04

# Install deps
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    ninja-build \
    wget \
    unzip \
    libncurses5 \
    && rm -rf /var/lib/apt/lists/*

# Install ARM GCC toolchain
RUN wget -q https://developer.arm.com/-/media/Files/downloads/gnu-rm/10.3-2021.10/gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2 \
    && tar -xjf gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2 -C /opt \
    && rm gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2

ENV PATH="/opt/gcc-arm-none-eabi-10.3-2021.10/bin:$PATH"

WORKDIR /workspace
