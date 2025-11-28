#!/bin/bash

arm-none-eabi-gcc components/core/src/main.c components/core/src/startup_stm32wle5jcix.s vendor/cmsis-device-wl/Source/Templates/system_stm32wlxx.c -T STM32WLE5JCIX_FLASH.ld -o blink.elf -Ivendor/CMSIS_5/CMSIS/Core/Include -Ivendor/cmsis-device-wl/Include -mcpu=cortex-m4 -mthumb -nostdlib -DSTM32WLE5xx 
