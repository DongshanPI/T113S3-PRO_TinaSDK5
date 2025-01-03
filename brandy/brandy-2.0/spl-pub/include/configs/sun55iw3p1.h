/*
 * (C) Copyright 2013-2019
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *
 */
#ifndef _SUN55IW3_H
#define _SUN55IW3_H


//#define FPGA_PLATFORM
#define BOOT_PUB_HEAD_VERSION  "3000"
#define CONFIG_ARCH_SUN55IW3
#define CONFIG_ARCH_SUN55IW3P1
#define CONFIG_DRAM_PARA_V1
#define CONFIG_MONITOR
#define SCP_DEASSERT_BY_MONITOR
#define CONFIG_SUNXI_VERSION_AOTUGEN 1
/*#define FPGA_PLATFORM*/

/* sram layout*/
#define CONFIG_SYS_SRAM_BASE             (0x20000)
#define CONFIG_SYS_SRAM_SIZE             (0x10000)
#define CONFIG_SYS_SRAMA2_BASE           (0x40000) /*(SUNXI_SRAM_A2_BASE)*/
#define CONFIG_SYS_SRAMA2_SIZE           (0x24000)
/* dram layout*/
#define SDRAM_OFFSET(x)                   (0x40000000+(x))
#define CONFIG_SYS_DRAM_BASE              SDRAM_OFFSET(0)
#define DRAM_PARA_STORE_ADDR              SDRAM_OFFSET(0x00800000) /*fel*/
#define CONFIG_HEAP_BASE                  SDRAM_OFFSET(0x00800000) /*secure */
#define CONFIG_HEAP_SIZE                  (16 * 1024 * 1024)

#define CONFIG_BOOTPKG_BASE               SDRAM_OFFSET(0x02e00000)
#define CONFIG_MONITOR_BASE               SDRAM_OFFSET(0x08000000)
#define SCP_DRAM_BASE                     SDRAM_OFFSET(0x08100000)

#define SUNXI_DRAM_PARA_MAX               32

#define SUNXI_LOGO_COMPRESSED_LOGO_SIZE_ADDR            SDRAM_OFFSET(0x03000000)
#define SUNXI_LOGO_COMPRESSED_LOGO_BUFF                 SDRAM_OFFSET(0x03000010)
#define SUNXI_SHUTDOWN_CHARGE_COMPRESSED_LOGO_SIZE_ADDR SDRAM_OFFSET(0x03100000)
#define SUNXI_SHUTDOWN_CHARGE_COMPRESSED_LOGO_BUFF  	SDRAM_OFFSET(0x03100010)
#define SUNXI_ANDROID_CHARGE_COMPRESSED_LOGO_SIZE_ADDR  SDRAM_OFFSET(0x03200000)
#define SUNXI_ANDROID_CHARGE_COMPRESSED_LOGO_BUFF   	SDRAM_OFFSET(0x03200010)


/* scp mem layout */
#define SCP_DRAM_SIZE                    (0x8000)
#define SCP_CODE_DRAM_OFFSET		     (0x24000)
#define SCP_SRAM_BASE                    (CONFIG_SYS_SRAMA2_BASE)
#define SCP_SRAM_SIZE                    (CONFIG_SYS_SRAMA2_SIZE)
#define SCP_TEMP_STORE_BASE              (SCP_DRAM_BASE)
#define HEADER_OFFSET                    (0x4000)
/*dram_para_offset is the numbers of u32 before dram data sturcture(dram_para) in struct arisc_para*/
#define SCP_DRAM_PARA_OFFSET  (sizeof(u32) * 2)
#define SCP_DARM_PARA_NUM     (32)


/* boot run addr */
#define FEL_BASE                          0x20
#define SECURE_FEL_BASE                  (0x20)
#define CONFIG_BOOT0_RUN_ADDR            (CONFIG_SYS_SRAMA2_BASE + 0x4000)  /* sram a */
#define CONFIG_NBOOT_STACK               (CONFIG_SYS_SRAMA2_BASE+CONFIG_SYS_SRAMA2_SIZE)
#define CONFIG_TOC0_RUN_ADDR             (CONFIG_BOOT0_RUN_ADDR + 0x480)  /* sram a */
#define CONFIG_HASH_TABLE_STACK_GAP      (4)
#define CONFIG_HASH_INFO_TABLE_SIZE      (512 - CONFIG_HASH_TABLE_STACK_GAP)/*to pass hash info to optee*/
#define CONFIG_HASH_INFO_TABLE_BASE      (CONFIG_SYS_SRAMA2_BASE + CONFIG_SYS_SRAMA2_SIZE - CONFIG_HASH_INFO_TABLE_SIZE)
#define CONFIG_SBOOT_STACK               (CONFIG_HASH_INFO_TABLE_BASE - CONFIG_HASH_TABLE_STACK_GAP)
#define CONFIG_BOOT0_RET_ADDR            (CONFIG_BOOT0_RUN_ADDR)
#define CONFIG_TOC0_HEAD_BASE            (CONFIG_BOOT0_RUN_ADDR)
#define CONFIG_TOC0_CFG_ADDR             (CONFIG_BOOT0_RUN_ADDR + 0x80)


#define CONFIG_SYS_INIT_RAM_SIZE 0x8000

/* FES */
#define CONFIG_FES1_RUN_ADDR             (CONFIG_BOOT0_RUN_ADDR + 0x8000)
#define CONFIG_FES1_RET_ADDR             (CONFIG_BOOT0_RUN_ADDR + 0x7210)

/*CPU vol for boot*/
#define CONFIG_SUNXI_CORE_VOL           1100

#define CONFIG_SUNXI_SYS_VOL           950
/*R_GPIO: PL0,PL1 cfg*/
#define PL_CTL_REG0 (0x22)

#define SPIC0_BASE_ADDR			(0x04025000)
#define CCMU_BASE_ADDR			(0x02001000)
#define GPIO_BASE_ADDR			(0x02000000)
#define SPI_CLK_PLL_PERI0		(300000000)

#endif
