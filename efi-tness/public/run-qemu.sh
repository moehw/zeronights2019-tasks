#!/bin/sh

qemu-system-x86_64 \
    -monitor /dev/null \
    -L . --bios ./ovmf/OVMF.fd \
    -m 64M \
    -net none \
    -drive file=fat:./efitness,media=disk,if=virtio,format=raw,readonly=on \
    -nographic