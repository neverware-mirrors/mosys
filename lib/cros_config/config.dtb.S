.section .dtb.init.rodata,"a"
.balign 16
.global __dtb_config_begin
__dtb_config_begin:
.incbin "lib/cros_config/config.dtb"
__dtb_config_end:
.global __dtb_config_end
.balign 16