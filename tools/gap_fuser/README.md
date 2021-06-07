# GAP8 Fuser

This is a fuser example for programming the efuse on GAP8. 
BE CAREFUL!!!! Fuse is an one shoot mechanism, which means this will permanently configure the chip and cannot be undo.

For further information about fuse map, please read: https://greenwaves-technologies.com/wp-content/uploads/2021/03/AN004_BootFromFlash.pdf

According to the fuse map, you can configure your fuse map by using the header file: fuser_map.h 

# Examples:
## Burn the efuse for GAP8_REVC with HYPER Flash
~~~~~sh
make clean all run revc_hyper=1
~~~~~

## Burn the efuse for GAP8_REVC with QSPI Flash
~~~~~sh
make clean all run revc_qspi=1
~~~~~

## Burn the efuse for GAP8_REVB with HYPER Flash
~~~~~sh
make clean all run revb_hyper=1
~~~~~

## Dump ONLY the registers have been fused
~~~~~sh
make clean all run
~~~~~

or

~~~~~sh
make clean all run dump=1
~~~~~

## Dump ALL the registers
~~~~~sh
make clean all run dump=2
~~~~~


