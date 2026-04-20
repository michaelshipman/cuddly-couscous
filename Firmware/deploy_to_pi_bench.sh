#!/bin/bash

scons -c
scons
scp build/program.elf shipman@pi-bench.local:/home/shipman/Projects/cuddly-couscous/Firmware/build/
scp build/program.map shipman@pi-bench.local:/home/shipman/Projects/cuddly-couscous/Firmware/build/
