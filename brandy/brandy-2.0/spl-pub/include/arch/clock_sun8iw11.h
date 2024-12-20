/*
 * (C) Copyright 2007-2015
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Jerry Wang <wangflord@allwinnertech.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 */

#ifndef __CCMU_H
#define __CCMU_H

#include <config.h>
#include <arch/cpu.h>

/* pll list */
#define CCMU_PLL_CPUX_CTRL_REG (SUNXI_CCM_BASE + 0x00)
#define CCMU_PLL_C0CPUX_CTRL_REG CCMU_PLL_CPUX_CTRL_REG
#define CCMU_PLL_C1CPUX_CTRL_REG (SUNXI_CCM_BASE + 0x04)
#define CCMU_PLL_AUDIO_CTRL_REG (SUNXI_CCM_BASE + 0x08)
#define CCMU_PLL_VIDEO0_CTRL_REG (SUNXI_CCM_BASE + 0x10)
#define CCMU_PLL_VE_CTRL_REG (SUNXI_CCM_BASE + 0x18)
#define CCMU_PLL_DDR0_CTRL_REG (SUNXI_CCM_BASE + 0x20)
#define CCMU_PLL_PERIPH0_CTRL_REG (SUNXI_CCM_BASE + 0x28)
#define CCMU_PLL_PERIPH1_CTRL_REG (SUNXI_CCM_BASE + 0x2C)
#define CCMU_PLL_VIDEO1_CTRL_REG (SUNXI_CCM_BASE + 0x30)
#define CCMU_PLL_GPU_CTRL_REG (SUNXI_CCM_BASE + 0x38)
#define CCMU_PLL_MIPI_CTRL_REG (SUNXI_CCM_BASE + 0x40)

#define CCMU_PLL_HSIC_CTRL_REG (SUNXI_CCM_BASE + 0x44)
#define CCMU_PLL_DE_CTRL_REG (SUNXI_CCM_BASE + 0x48)
#define CCMU_PLL_DDR1_CTRL_REG (SUNXI_CCM_BASE + 0x4C)

/* new mode for pll lock */
#define CCMU_PLL_LOCK_CTRL_REG (SUNXI_CCM_BASE + 0x320)
#define LOCK_EN_PLL_CPUX (1 << 0)
#define LOCK_EN_PLL_AUDIO (1 << 1)
#define LOCK_EN_PLL_VIDEO0 (1 << 2)
#define LOCK_EN_PLL_VE (1 << 3)
#define LOCK_EN_PLL_DDR0 (1 << 4)
#define LOCK_EN_PLL_PERIPH0 (1 << 5)
#define LOCK_EN_PLL_VIDEO1 (1 << 6)
#define LOCK_EN_PLL_GPU (1 << 7)
#define LOCK_EN_PLL_MIPI (1 << 8)
#define LOCK_EN_PLL_HSIC (1 << 9)
#define LOCK_EN_PLL_DE (1 << 10)
#define LOCK_EN_PLL_DDR1 (1 << 11)
#define LOCK_EN_PLL_PERIPH1 (1 << 12)
#define LOCK_EN_NEW_MODE (1 << 28)

/* cfg list */
#define CCMU_CPUX_AXI_CFG_REG (SUNXI_CCM_BASE + 0x50)
#define CCMU_AHB1_APB1_CFG_REG (SUNXI_CCM_BASE + 0x54)
#define CCMU_APB2_CFG_GREG (SUNXI_CCM_BASE + 0x58)
#define CCMU_AHB2_CFG_GREG (SUNXI_CCM_BASE + 0x5C)

/* gate list */
#define CCMU_BUS_CLK_GATING_REG0 (SUNXI_CCM_BASE + 0x60)
#define CCMU_BUS_CLK_GATING_REG1 (SUNXI_CCM_BASE + 0x64)
#define CCMU_BUS_CLK_GATING_REG2 (SUNXI_CCM_BASE + 0x68)
#define CCMU_BUS_CLK_GATING_REG3 (SUNXI_CCM_BASE + 0x6C)
#define CCMU_BUS_CLK_GATING_REG4 (SUNXI_CCM_BASE + 0x70)

#define CCMU_CCI400_CFG_REG (SUNXI_CCM_BASE + 0x78)

/* module list */
#define CCMU_NAND0_CLK_REG (SUNXI_CCM_BASE + 0x80)
#define CCMU_SDMMC0_CLK_REG (SUNXI_CCM_BASE + 0x88)
#define CCMU_SDMMC1_CLK_REG (SUNXI_CCM_BASE + 0x8C)
#define CCMU_SDMMC2_CLK_REG (SUNXI_CCM_BASE + 0x90)
#define CCMU_SDMMC3_CLK_REG (SUNXI_CCM_BASE + 0x94)

#define CCMU_CE_CLK_REG (SUNXI_CCM_BASE + 0x9C)
#define CCMU_SPI0_SCLK_CTRL (SUNXI_CCM_BASE + 0xA0)

#define CCMU_USBPHY_CLK_REG (SUNXI_CCM_BASE + 0xCC)
#define CCMU_DRAM_CLK_REG (SUNXI_CCM_BASE + 0xF4)
#define CCMU_PLL_DDR_CFG_REG (SUNXI_CCM_BASE + 0xF8)
#define CCMU_MBUS_RST_REG (SUNXI_CCM_BASE + 0xFC)
#define CCMU_DRAM_CLK_GATING_REG (SUNXI_CCM_BASE + 0x100)

#define CCMU_AVS_CLK_REG (SUNXI_CCM_BASE + 0x144)
#define CCMU_MBUS_CLK_REG (SUNXI_CCM_BASE + 0x15C)

#define CCMU_PLL_STB_STATUS_REG (SUNXI_CCM_BASE + 0x20C)
#define CCMU_PLL_C0CPUX_BIAS_REG (SUNXI_CCM_BASE + 0x220)
/*gate rst list*/
#define CCMU_BUS_SOFT_RST_REG0 (SUNXI_CCM_BASE + 0x2C0)
#define CCMU_BUS_SOFT_RST_REG1 (SUNXI_CCM_BASE + 0x2C4)
#define CCMU_BUS_SOFT_RST_REG2 (SUNXI_CCM_BASE + 0x2C8)
#define CCMU_BUS_SOFT_RST_REG3 (SUNXI_CCM_BASE + 0x2D0)
#define CCMU_BUS_SOFT_RST_REG4 (SUNXI_CCM_BASE + 0x2D8)

#define CCMU_SEC_SWITCH_REG (SUNXI_CCM_BASE + 0x2F0)

#define CCMU_AHB1_RST_REG0 (CCMU_BUS_SOFT_RST_REG0)
#define CCMU_AHB1_GATE0_CTRL (CCMU_BUS_CLK_GATING_REG0)

#define CCMU_PLL_PERI0_CTRL_REG CCMU_PLL_PERIPH0_CTRL_REG

/* #define CCMU_AHB1_RST_REG0      (SUNXI_CCM_BASE+0x02C0) */
#define CCMU_AHB1_RST_REG1 (SUNXI_CCM_BASE + 0x02C4)
#define CCMU_AHB1_RST_REG2 (SUNXI_CCM_BASE + 0x02C8)
#define CCMU_APB1_RST_REG (SUNXI_CCM_BASE + 0x02D0)

/*CE*/
#define CE_CLK_SRC_MASK (0x3)
#define CE_CLK_SRC_SEL_BIT (24)
#define CE_CLK_SRC (0x01)

#define CE_CLK_DIV_RATION_N_BIT (16)
#define CE_CLK_DIV_RATION_N_MASK (0x3)
#define CE_CLK_DIV_RATION_N (0)

#define CE_CLK_DIV_RATION_M_BIT (0)
#define CE_CLK_DIV_RATION_M_MASK (0xF)
#define CE_CLK_DIV_RATION_M (3)

#define CE_SCLK_ONOFF_BIT (31)
#define CE_SCLK_ON (1)

#define CE_GATING_BASE CCMU_BUS_CLK_GATING_REG0
#define CE_GATING_PASS (1)
#define CE_GATING_BIT (5)

#define CE_RST_REG_BASE CCMU_BUS_SOFT_RST_REG0
#define CE_RST_BIT (5)
#define CE_DEASSERT (1)

/*DMA*/
#define DMA_GATING_BASE CCMU_BUS_CLK_GATING_REG0
#define DMA_GATING_PASS (1)
#define DMA_GATING_BIT (6)
#define AHB_GATE_OFFSET_DMA		6

/*for other file ,use before define*/
#define CCM_AVS_SCLK_CTRL (CCMU_AVS_CLK_REG)
#define CCM_AHB1_GATE0_CTRL (CCMU_BUS_CLK_GATING_REG0)
#define CCM_AHB1_RST_REG0 (CCMU_BUS_SOFT_RST_REG0)

/* clock ID */
#define AXI_BUS (0)
#define AHB1_BUS0 (1)
#define AHB1_BUS1 (2)
#define AHB1_LVDS (3)
#define APB1_BUS0 (4)
#define APB2_BUS0 (5)

/* ehci */
#define BUS_CLK_GATING_REG 0x60
#define BUS_SOFTWARE_RESET_REG 0x2c0
#define USBPHY_CONFIG_REG 0xcc

#define USBEHCI0_RST_BIT 24
#define USBEHCI0_GATIING_BIT 24
#define USBPHY0_RST_BIT 0
#define USBPHY0_SCLK_GATING_BIT 8

#define USBEHCI1_RST_BIT 25
#define USBEHCI1_GATIING_BIT 25
#define USBPHY1_RST_BIT 1
#define USBPHY1_SCLK_GATING_BIT 9

#define SPI3_CKID ((AHB1_BUS0 << 8) | 23)
#define SPI2_CKID ((AHB1_BUS0 << 8) | 22)
#define SPI1_CKID ((AHB1_BUS0 << 8) | 21)
#define SPI0_CKID ((AHB1_BUS0 << 8) | 20)

/* SPI CONFIG */
#define SPI_RST_OFFSET (20)
#define SPI_GATING_OFFSET (20)

#define CCMU_SPI0_SCLK_CTRL (SUNXI_CCM_BASE + 0xA0)
#define CCMU_SPI1_SCLK_CTRL (SUNXI_CCM_BASE + 0xA4)

/* handle standby */
#define RTC_STANDBY_FLAG_REG (SUNXI_RTC_BASE + 0x1f8)
#define RTC_STANDBY_SOFT_ENTRY_REG (SUNXI_RTC_BASE + 0x1fc)

/* for dram crc check */
#define CRC_VALUE_BEFORE (3)
#define CRC_EN (0)
#define CRC_START (6)
#define CRC_LEN (7)

#endif
