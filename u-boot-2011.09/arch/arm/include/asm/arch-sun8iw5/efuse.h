/*
**********************************************************************************************************************
*
*						           the Embedded Secure Bootloader System
*
*
*						       Copyright(C), 2006-2014, Allwinnertech Co., Ltd.
*                                           All Rights Reserved
*
* File    :
*
* By      :
*
* Version : V2.00
*
* Date	  :
*
* Descript:
*SPDX-License-Identifier:	GPL-2.0+
**********************************************************************************************************************
*/

#ifndef __EFUSE_H__
#define __EFUSE_H__

#define SUNXI_SID_BASE				0x01c23800

#define SID_PDKEY				(SUNXI_SID_BASE + 0x0)
#define SID_PRCTL				(SUNXI_SID_BASE + 0x40)
#define SID_PRKEY				(SUNXI_SID_BASE + 0x50)


#define EFUSE_CHIPID			(0x00)
#define EFUSE_THERMAL_SENSOR	(0x10)
#define EFUSE_CHIPCONFIG		(0x14)
#define EFUSE_BACKUP_KEY		(0x18)
#define EFUSE_ROOT_KEY			(0x20)

#define EFUSE_OEM_ID			EFUSE_BACKUP_KEY

#endif    /*  #ifndef __EFUSE_H__  */
