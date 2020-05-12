#!/usr/bin/python3
import os
import tempfile
import signal
import subprocess

hd = None

def sighandler(signum, frame):
    hd.kill()

signal.signal(signal.SIGHUP, sighandler)

fname = tempfile.NamedTemporaryFile().name

os.system('cd /home/efitness/deploy/')
os.system("cp /home/efitness/binary/ovmf/OVMF.fd %s" % (fname))
os.system("chmod u+w %s" % (fname))

hd = subprocess.Popen("qemu-system-x86_64 \
    -L . --bios /home/efitness/binary/ovmf/OVMF.fd \
    -net none \
    -m 64M \
    -drive file=fat:/home/efitness/binary/efitness,media=disk,if=virtio,format=raw,readonly=on \
    -monitor /dev/null -drive if=pflash,format=raw,file=%s \
    -nographic 2> /dev/null" % (fname), shell=True)

try:
    hd.wait(30)
except subprocess.TimeoutExpired as e:
    hd.kill()
    print('Timeout!')

os.system("rm -rf %s" % (fname))
