# Setup prefix where riscv tools will go
# First stage, build risc-v toolchain

FROM ubuntu:22.04

RUN apt update
RUN apt install -y \
  build-essential sudo vim software-properties-common openssh-server \
  gcc-multilib g++-multilib llvm sysstat make perl clang \
  git autoconf flex texinfo help2man gawk libtool \
  libtool-bin bison python3-dev python3.10-dev \
  pkg-config unzip device-tree-compiler libboost-regex-dev \
  libboost-system-dev

RUN useradd -ms /bin/bash researcher -G sudo
USER researcher
WORKDIR /home/researcher
ENV RISCV_PREFIX="/home/researcher/x-tools/riscv32-unknown-elf"

# Don't let apt prompt for interaction
ENV DEBIAN_FRONTEND noninteractive

USER root
USER researcher

# Crosstool ##########################
WORKDIR /home/researcher
COPY --chown=researcher:researcher crosstool-ng crosstool-ng
WORKDIR /home/researcher/crosstool-ng

RUN ./bootstrap && ./configure --enable-local
RUN make -j$(nproc)
COPY --chown=researcher:researcher crosstool-defconfig ./defconfig
RUN ./ct-ng defconfig
# Run ct-ng with all available cores and remove intermediate build files to save space
RUN ./ct-ng build.$(nproc) \
  && rm -rf /home/researcher/crosstool-ng/.build

# OpenOCD ##########################
WORKDIR /home/researcher
COPY --chown=researcher:researcher riscv-openocd riscv-openocd
WORKDIR /home/researcher/riscv-openocd 

RUN ./bootstrap && ./configure --prefix=$RISCV_PREFIX
# Build with all available cores
RUN make -j$(nproc)
# For some reason make install's permissions are messed up and want root despite being installed to a user directory
USER root
RUN make install
USER researcher

# FreeRTOS for SPIKE ##########################
WORKDIR /home/researcher
COPY --chown=researcher:researcher FreeRTOS FreeRTOS
WORKDIR /home/researcher/FreeRTOS/FreeRTOS/Demo/RISC-V-spike-htif_GCC

# Update path to include toolchain binaries
ENV PATH "/home/researcher/bin:$RISCV_PREFIX/bin:$PATH"
RUN sed -i "s/MARCH\s*=.*/MARCH = rv32ima_zicsr_zifencei/" Makefile
# Build with all available cores
RUN make -j$(nproc)

# SPIKE Simulator ##########################
WORKDIR /home/researcher
COPY --chown=researcher:researcher riscv-isa-sim riscv-isa-sim
WORKDIR /home/researcher/riscv-isa-sim

RUN mkdir build
WORKDIR build
RUN ../configure --prefix=$RISCV_PREFIX
# Build with all available cores
RUN make -j$(nproc)
# For some reason make install's permissions are messed up and want root despite being installed to a user directory
USER root
RUN make install
USER researcher

# Final Setup ########################
COPY --chown=researcher:researcher tests /home/researcher/FreeRTOS/FreeRTOS/tests
COPY --chown=researcher:researcher scripts /home/researcher/bin

WORKDIR /home/researcher/FreeRTOS/FreeRTOS/Demo/RISC-V-spike-htif_GCC

