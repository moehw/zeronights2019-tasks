#!/bin/bash

qemu-system-x86_64 \
    -monitor /dev/null \
    -L . --bios /home/efitness/binary/ovmf/OVMF.fd \
    -m 64M \
    -net none \
    -drive file=fat:/home/efitness/binary/efitness,media=disk,if=virtio,format=raw,readonly=on \
    -nographic 2> /dev/null