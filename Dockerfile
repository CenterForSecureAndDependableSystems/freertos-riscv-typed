FROM u1f98e/crosstools-riscv32-zicsr

# Don't let apt prompt for interaction
ENV DEBIAN_FRONTEND noninteractive

RUN apt update
RUN apt install -y \
  build-essential sudo vim software-properties-common openssh-server \
  gcc-multilib g++-multilib llvm sysstat make perl clang \
  git autoconf flex texinfo help2man gawk libtool \
  libtool-bin bison python3-dev python3.10-dev \
  pkg-config unzip device-tree-compiler libboost-regex-dev \
  libboost-system-dev

RUN useradd -ms /bin/bash researcher -G sudo
WORKDIR /home/researcher
ENV RISCV_PREFIX="/opt/x-tools/riscv32-unknown-elf"

# OpenOCD ##########################
USER researcher
WORKDIR /home/researcher
COPY --chown=researcher:researcher riscv-openocd riscv-openocd
WORKDIR /home/researcher/riscv-openocd 

RUN ./bootstrap nosubmodule && ./configure --prefix=$RISCV_PREFIX
# Build with all available cores
RUN make -j$(nproc)
# For some reason make install's permissions are messed up and want root despite being installed to a user directory
USER root
RUN make install

# FreeRTOS for SPIKE ##########################
USER researcher
WORKDIR /home/researcher
COPY --chown=researcher:researcher FreeRTOS FreeRTOS
WORKDIR /home/researcher/FreeRTOS/FreeRTOS/Demo/RISC-V-spike-htif_GCC

# Update path to include toolchain binaries
RUN sed -i "s/MARCH\s*=.*/MARCH = rv32ima_zicsr_zifencei/" Makefile
# Build with all available cores
RUN make -j$(nproc)

# SPIKE Simulator ##########################
WORKDIR /home/researcher
COPY --chown=researcher:researcher riscv-isa-sim-typed riscv-isa-sim-typed
WORKDIR /home/researcher/riscv-isa-sim-typed

RUN mkdir build
WORKDIR build
RUN ../configure --prefix=$RISCV_PREFIX
# Build with all available cores
RUN make -j$(nproc)
# For some reason make install's permissions are messed up and want root despite being installed to a user directory
USER root
RUN make install

# libtypetag ########################
USER researcher
WORKDIR /home/researcher
COPY --chown=researcher:researcher libtypetag libtypetag
WORKDIR /home/researcher/libtypetag
RUN make -j$(nproc) 
USER root
RUN make install DESTDIR=$RISCV_PREFIX
USER researcher

# Final Setup ########################
COPY --chown=researcher:researcher tests /home/researcher/FreeRTOS/FreeRTOS/tests
COPY scripts /usr/local/bin

WORKDIR /home/researcher/FreeRTOS/FreeRTOS/tests
USER researcher
