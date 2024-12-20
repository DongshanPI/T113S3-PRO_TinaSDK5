
#
#config file for sun8iw19
#
SUPPORT_BOARD=v833 v831 v535 v533 v459
ARCH = arm
PLATFORM = sun8iw19p1

CFG_BOOT0_RUN_ADDR=0x20000
CFG_SYS_INIT_RAM_SIZE=0x18000
CFG_FES1_RUN_ADDR=0x28000
CFG_SBOOT_RUN_ADDR=0x20480

#pmu
CFG_SUNXI_POWER=y
CFG_SUNXI_TWI=y
CFG_SUNXI_PMIC=y
CFG_AXP2101_POWER=y
CFG_AXP152_POWER=y
CFG_SUNXI_WATCHDOG=y

#chipid
CFG_SUNXI_CHIPID=y

#standby
CFG_SUNXI_STANDBY=y

#dma
CFG_SUNXI_DMA=y

#spi
#CFG_SPI_USE_DMA=y
