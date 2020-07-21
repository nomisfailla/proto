# OSDev Starter Kit
This repository contains the minimal code needed to compile and boot a Multiboot 2 compiant kernel using GRUB.

## Dependencies
```
sudo apt install g++ binutils libc6-dev-i386 nasm
sudo apt install grub-pc-bin grub-common xorriso
```

## Build Instructions
```
git clone https://github.com/nomisfailla/osdev-starter.git
cd osdev-starter
make
```
This will create an iso file named `release.iso` which can be booted in an emulator using the supplied `.bochsrc` or burnt to a disk.
