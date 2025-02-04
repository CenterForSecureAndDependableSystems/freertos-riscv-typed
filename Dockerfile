FROM u1f98e/crosstools-riscv32-zicsr

ARG PROJECT_DIR="/home/researcher/project"
ENV RISCV_PREFIX="/opt/x-tools/riscv32-unknown-elf"
# Don't let apt prompt for interaction
ENV DEBIAN_FRONTEND=noninteractive

RUN apt update && apt install -y \
  build-essential sudo vim software-properties-common openssh-server \
  gcc-multilib g++-multilib gdb llvm sysstat make perl clang \
  git autoconf flex texinfo help2man gawk libtool \
  libtool-bin bison python3-dev python3.10-dev \
  pkg-config unzip device-tree-compiler libboost-regex-dev \
  libboost-system-dev

RUN useradd -ms /bin/bash researcher -G sudo
USER researcher
RUN mkdir -p $PROJECT_DIR
WORKDIR $PROJECT_DIR

# OpenOCD ##########################
USER researcher
WORKDIR $PROJECT_DIR
COPY --chown=researcher:researcher riscv-openocd riscv-openocd
WORKDIR $PROJECT_DIR/riscv-openocd 

RUN ./bootstrap nosubmodule && ./configure --prefix=$RISCV_PREFIX
# Build with all available cores
RUN make -j$(nproc)
# For some reason make install's permissions are messed up and want root despite being installed to a user directory
USER root
RUN make install

# FreeRTOS for SPIKE ##########################
USER researcher
WORKDIR $PROJECT_DIR
COPY --chown=researcher:researcher FreeRTOS FreeRTOS
WORKDIR $PROJECT_DIR/FreeRTOS/FreeRTOS/Demo/RISC-V-spike-htif_GCC

# Update path to include toolchain binaries
RUN sed -i "s/MARCH\s*=.*/MARCH = rv32ima_zicsr_zifencei/" Makefile
# Build with all available cores
RUN make -j$(nproc)

# libtypetag ########################
USER researcher
WORKDIR $PROJECT_DIR
COPY --chown=researcher:researcher libtypetag libtypetag
WORKDIR $PROJECT_DIR/libtypetag
# We need to build versions for the demos (riscv) and x86 (for spike)
# Build riscv libtypetag
RUN make -j$(nproc) ARCH=riscv
# Build x86 libtypetag
RUN make -j$(nproc)
# Install libraries
USER root
RUN make install # Install for x86 (for SPIKE)
RUN make install ARCH=riscv DESTDIR=$RISCV_PREFIX # Install for riscv (demos)
USER researcher

# SPIKE Simulator ##########################
WORKDIR $PROJECT_DIR
COPY --chown=researcher:researcher riscv-isa-sim-typed riscv-isa-sim-typed
WORKDIR $PROJECT_DIR/riscv-isa-sim-typed

RUN mkdir build
WORKDIR build
RUN ../configure
# Build with all available cores
RUN make -j$(nproc)
# For some reason make install's permissions are messed up and want root despite being installed to a user directory
USER root
RUN make install

# Final Setup ########################
COPY --chown=researcher:researcher tests $PROJECT_DIR/tests
COPY scripts /usr/local/bin

# For in-container development only (copy source control and other files)
COPY --chown=researcher:researcher . $PROJECT_DIR

WORKDIR $PROJECT_DIR
USER researcher
