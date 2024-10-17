# Setup prefix where riscv tools will go
# First stage, build risc-v toolchain

FROM ubuntu:22.04 as build

# Don't let apt prompt for interaction
ENV DEBIAN_FRONTEND noninteractive

RUN apt update && apt install -y \
  autoconf bison build-essential clang flex g++-multilib \
  gawk gcc-multilib git help2man libtool libtool-bin llvm \
  make openssh-server perl pkg-config python3-dev python3.10-dev \
  software-properties-common sysstat texinfo unzip
  
RUN useradd -ms /bin/bash crosstool
WORKDIR /home/crosstool

# Crosstool ##########################
COPY --chown=crosstool:crosstool crosstool-ng crosstool-ng
WORKDIR /home/crosstool/crosstool-ng

USER crosstool
RUN ./bootstrap

RUN ./configure --enable-local
RUN make -j$(nproc)

COPY --chown=crosstool:crosstool crosstool-defconfig defconfig
RUN ./ct-ng defconfig
# Run ct-ng with all available cores and remove intermediate build files to save space
RUN ./ct-ng build.$(nproc)

FROM ubuntu:22.04
COPY --from=build /home/crosstool/x-tools /opt/x-tools
ENV PATH "/opt/x-tools/riscv32-unknown-elf/bin:$PATH"

