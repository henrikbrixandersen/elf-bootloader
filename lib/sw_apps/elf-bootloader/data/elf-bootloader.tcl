# Copyright (c) 2017 Henrik Brix Andersen <henrik@brixandersen.dk>
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
#

proc swapp_get_name {} {
    return "elf-bootloader";
}

proc swapp_get_description {} {
    return "ELF first-stage bootloader for microblaze by Henrik Brix Andersen.";
}

proc swapp_is_supported_hw {} {
    set proc_instance [hsi::get_sw_processor];
    set hw_processor [common::get_property HW_INSTANCE $proc_instance]

    set proc_type [common::get_property IP_NAME [hsi::get_cells -hier $hw_processor]];

    if { $proc_type != "microblaze" } {
        error "This application is supported only for microblaze processors.";
    }

    return 1;
}

proc swapp_is_supported_sw {} {
    set oslist [hsi::get_os];

    if { [llength $oslist] != 1 } {
        return 0;
    }
    set os [lindex $oslist 0];

    if { $os != "standalone"} {
        error "This application is supported only on the standalone Board Support Package.";
    }
}

proc swapp_generate {} {

}
