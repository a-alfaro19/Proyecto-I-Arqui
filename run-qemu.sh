#!/bin/bash

# Run QEMU with GDB server
BIN_DIR=bin
ELF=$BIN_DIR/program.elf

echo "Starting QEMU with GDB server on port 1234..."
echo "In another terminal, run: gdb-multiarch $ELF"
echo "Then in GDB: target remote :1234"
echo ""
echo "Useful GDB commands for this example:"
echo "  break _start          - break at C program start"
echo "  break sum_to_n        - break at assembly function"
echo "  info registers        - show register values"
echo "  step / next           - step through code"

qemu-system-riscv32 \
    -machine virt \
    -nographic \
    -bios none \
    -kernel $ELF \
    -S \
    -gdb tcp::1234