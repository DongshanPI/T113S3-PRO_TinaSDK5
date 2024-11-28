/*
 * (C) Copyright 2013-2016
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *
 */

#ifndef __EFUSE_H__
#define __EFUSE_H__

#include <arch/cpu.h>

#define SID_PRCTL               (SUNXI_SID_BASE + 0x40)
#define SID_PRKEY               (SUNXI_SID_BASE + 0x50)
#define SID_RDKEY               (SUNXI_SID_BASE + 0x60)
#define SJTAG_AT0               (SUNXI_SID_BASE + 0x80)
#define SJTAG_AT1               (SUNXI_SID_BASE + 0x84)
#define SJTAG_S                 (SUNXI_SID_BASE + 0x88)

#define SID_EFUSE               (SUNXI_SID_BASE + 0x200)
#define SID_SECURE_MODE         (SUNXI_SID_BASE + 0xA0)
#define SID_OP_LOCK  (0xAC)

/*efuse power ctl*/
#define EFUSE_HV_SWITCH			(SUNXI_RTC_BASE + 0x204)

#define EFUSE_CHIPD             (0x00) /* 0x0-0xf, 128bits */
#define EFUSE_BROM_CONFIG       (0x10) /* 16 bit config, 16 bits try */
#define EFUSE_THERMAL_SENSOR    (0x30) /* 0x14-0x1b, 64bits */
#define EFUSE_TF_ZONE           (0x28) /* 0x1c-0x2b, 128bits */
#define EFUSE_OEM_PROGRAM       (0x3C) /* 0x2c-0x3f, 160bits */

/* write protect */
#define EFUSE_WRITE_PROTECT     (0x80) /* 0x40-0x43, 32bits */
/* read  protect */
#define EFUSE_READ_PROTECT      (0x84) /* 0x44-0x47, 32bits */
/* jtag security */
#define EFUSE_LCJS              (0x88)
/* jtag attribute */
#define EFUSE_ATTR              (0x90)

/*0x50-0x6F
0x50-0x67: 192bits HUK hardware Unique key
0x68-0x6b: 32bits ID of operator
0x6C-0x6F: 32bits ID
*/
#define EFUSE_HUK               (0x94)
#define EFUSE_INDENTIFICATION   (0xAC)
#define EFUSE_ID                (0xB0)

#define EFUSE_ROTPK             (0xB4) /* 0x70-0x8f, 256bits */
#define EFUSE_SSK               (0xD4) /* 0x90-0x9f, 128bits */
#define EFUSE_RSSK              (0xF4) /* 0xA0-0xBf, 256bits */
#define EFUSE_SN                (0xB0) /* 0xB0-0xC7, 192bits */
#define EFUSE_NV1               (0x124) /* 0xC8-0xCB, 32 bits */
#define EFUSE_NV2               (0x128) /* 0xCC-0xCF, 32 bits */
#define EFUSE_HDCP_HASH         (0x114) /* 0xD0-0xDf, 128bits */
#define EFUSE_BACKUP_KEY        (0x164) /* 0xE0-0xF7, 192bits */
#define EFUSE_BACKUP_KEY2       (0x1A4) /* 0xF8-0xFF,  72bits */

/* size (bit)*/
#define SID_CHIPID_SIZE			(128)
#define SID_OEM_PROGRAM_SIZE		(512+32)
#define SID_NV1_SIZE			(32)
#define SID_NV2_SIZE			(224)
#define SID_RSAKEY_HASH_SIZE		(160)
#define SID_THERMAL_SIZE		(36)
#define SID_RENEWABILITY_SIZE		(64)
#define SID_IN_SIZE			(192)
#define SID_IDENTIFY_SIZE		(32)
#define SID_ID_SIZE			(32)
#define SID_ROTPK_SIZE			(256)
#define SID_SSK_SIZE			(256)
#define SID_RSSK_SIZE			(256)
#define SID_HDCP_HASH_SIZE		(128)
#define SID_SN_SIZE			(192)
#define SID_HUK_SIZE		(192)

#define EFUSE_HUK_PROTECT_OFFSET (9)


#endif    /*  #ifndef __EFUSE_H__  */
