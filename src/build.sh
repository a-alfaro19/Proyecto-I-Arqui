#!/bin/bash

# Build script for C+assembly example
echo "Building TEA Encryption program..."

# Compile C source to object file
riscv64-unknown-elf-gcc \
    -march=rv32im \
    -mabi=ilp32 \
    -nostdlib \
    -ffreestanding \
    -g3 \
    -gdwarf-4 \
    -c \
    main.c \
    -o main.o

if [ $? -ne 0 ]; then
    echo "C compilation failed"
    exit 1
fi

# Compile startup assembly to object file
riscv64-unknown-elf-gcc \
    -march=rv32im \
    -mabi=ilp32 \
    -nostdlib \
    -ffreestanding \
    -g3 \
    -gdwarf-4 \
    -c \
    startup.s \
    -o startup.o

if [ $? -ne 0 ]; then
    echo "Startup assembly compilation failed"
    exit 1
fi

# Compile tea_encrypt assembly source to object file
riscv64-unknown-elf-gcc \
    -march=rv32im \
    -mabi=ilp32 \
    -nostdlib \
    -ffreestanding \
    -g3 \
    -gdwarf-4 \
    -c \
    tea_encrypt.s \
    -o tea_encrypt.o

if [ $? -ne 0 ]; then
    echo "TEA Encrypt assembly compilation failed"
    exit 1
fi

# Compile tea_decrypt assembly source to object file
riscv64-unknown-elf-gcc \
    -march=rv32im \
    -mabi=ilp32 \
    -nostdlib \
    -ffreestanding \
    -g3 \
    -gdwarf-4 \
    -c \
    tea_decrypt.s \
    -o tea_decrypt.o

if [ $? -ne 0 ]; then
    echo "TEA Decrypt assembly compilation failed"
    exit 1
fi

# Link object files together
riscv64-unknown-elf-gcc \
    -march=rv32im \
    -mabi=ilp32 \
    -nostdlib \
    -ffreestanding \
    -g3 \
    -gdwarf-4 \
    startup.o \
    main.o \
    tea_encrypt.o \
    tea_decrypt.o \
    -T linker.ld \
    -o program.elf

if [ $? -eq 0 ]; then
    echo "Build successful: program.elf created"
    echo "Object files: startup.o, main.o, tea_encrypt.o, tea_decrypt.o"
else
    echo "Linking failed"
    exit 1
fi