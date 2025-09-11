#!/bin/bash

# Build script for C+assembly example
echo "Building TEA Encryption program..."

# --- Paths ---
SRC_DIR=src
BUILD_DIR=build
BIN_DIR=bin

# Create directories
mkdir -p $BUILD_DIR $BIN_DIR

# --- Compiler/Flags ---
CC=riscv64-unknown-elf-gcc
CFLAGS="-march=rv32im -mabi=ilp32 -nostdlib -ffreestanding -g3 -gdwarf-4 -c"
LDFLAGS="-march=rv32im -mabi=ilp32 -nostdlib -ffreestanding -g3 -gdwarf-4"

# --- Sources ---
SRC_C="main.c"
SRC_ASM=("startup.s" "tea_encrypt.s" "tea_decrypt.s")

# --- Compile C source ---
echo "Compiling $SRC_C..."
$CC $CFLAGS $SRC_DIR/$SRC_C -o $BUILD_DIR/main.o

# --- Compile Assembly sources ---
for asm_file in "${SRC_ASM[@]}"; do
    obj_file="$BUILD_DIR/$(basename ${asm_file%.s}.o)"
    echo "Compiling $asm_file..."
    $CC $CFLAGS $SRC_DIR/$asm_file -o $obj_file
done

# --- Link ---
echo "Linking objects..."
$CC $LDFLAGS \
    $BUILD_DIR/startup.o \
    $BUILD_DIR/main.o \
    $BUILD_DIR/tea_encrypt.o \
    $BUILD_DIR/tea_decrypt.o \
    -T $SRC_DIR/linker.ld \
    -o $BIN_DIR/program.elf

echo "Build successful: $BIN_DIR/program.elf created"