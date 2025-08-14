FROM ubuntu:22.04

# Install dependencies
RUN apt-get update && \
    apt-get install -y git build-essential python3 python3-pip python3-venv \
                       pkg-config libglib2.0-dev libpixman-1-dev \
                       ninja-build wget curl && \
    apt-get clean

# Install RISC-V GNU toolchain (RV32 and RV64 support)
RUN apt-get update && \
    apt-get install -y gcc-riscv64-unknown-elf gdb-multiarch

# Create a non-root user (UID 1000 for convenience on most hosts)
RUN useradd -ms /bin/bash -u 1000 rvqemu-dev
USER rvqemu-dev
WORKDIR /home/rvqemu-dev

# Friendly prompt and PATH to local installs
RUN echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.bashrc && \
    echo 'export PS1="[\u@\h \W]\\$ "' >> ~/.bashrc

# Clone QEMU source (kept for students to modify)
RUN git clone https://gitlab.com/qemu-project/qemu.git /home/rvqemu-dev/qemu-src && \
    cd /home/rvqemu-dev/qemu-src && \
    git checkout stable-9.0

# Build & install QEMU to user prefix (no root needed later)
RUN cd /home/rvqemu-dev/qemu-src && \
    ./configure --target-list=riscv32-softmmu,riscv64-softmmu \
                --enable-debug --prefix=/home/rvqemu-dev/.local && \
    make -j"$(nproc)" && make install

# Helper script: rebuild QEMU after students edit device files
RUN printf '%s\n' \
'#!/usr/bin/env bash' \
'set -euo pipefail' \
'cd /home/rvqemu-dev/qemu-src' \
'./configure --target-list=riscv32-softmmu,riscv64-softmmu --enable-debug --prefix="$HOME/.local"' \
'make -j"$(nproc)"' \
'make install' \
> /home/rvqemu-dev/rebuild-qemu.sh && chmod +x /home/rvqemu-dev/rebuild-qemu.sh