#!/bin/bash
make
make ARCH=riscv

sudo make install
sudo make install ARCH=riscv
