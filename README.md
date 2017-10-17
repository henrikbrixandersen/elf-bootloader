# SPI ELF Bootloader
The SPI ELF Bootloader for Xilinx Microblaze processors can act as a first-stage bootloader (FSBL) for Xilinx Microblaze processors running either standalone application software, second-stage bootloaders, real-time operating systems (e.g. FreeRTOS) or or full-blown operating systems (e.g. GNU/Linux) in Executable and Linkable Format (ELF).

## Using the SPI ELF Bootloader
The SPI ELF bootloader is intended to be instantiated as a repository from within the Xilinx SDK. It has been tested to work with Xilinx Vivado 2017.3.

* Launch Xilinx SDK
* Choose *Repositories* from the *Xilinx* menu
* Add a new local or global repository and point it to the top-level directory of elf-bootloader (the one containing this *README.md* file)
* Create a new standalone *Application Project* by using the *File* menu
* Give it a meaningful name (e.g. fsbl) and click *Next*
* Choose *SPI ELF Bootloader* under *Available Templates* and click *Finish*
* Customize the generated *eb-config.h* file to match the SPI flash and flash address used in your project
* Ensure the generated application is set to execute from the correct memory (e.g. local memory if bootloading an application to external DDR memory)

# License
The SPI ELF Bootloader is available under the 2-clause BSD license:

Copyright (c) 2017 Henrik Brix Andersen <henrik@brixandersen.dk>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.
