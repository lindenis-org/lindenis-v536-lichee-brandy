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
* SPDX-License-Identifier:	GPL-2.0+
*
* Descript:
**********************************************************************************************************************
*/
#include "common.h"
#include "asm/io.h"
#include "asm/arch/efuse.h"

#define SID_OP_LOCK  (0xAC)

/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int sid_program_key(uint key_index, uint key_value)
{
	uint reg_val;
	if ((key_index < EFUSE_OEM_ID) || (key_index > EFUSE_ROOT_KEY)) {
		return -1;
	}

	writel(key_value, (SID_PRKEY + key_index));

	reg_val = readl(SID_PRCTL);
	reg_val &= ~((0xff << 8) | 0x3);
	reg_val |= (SID_OP_LOCK << 8) | 0x1;
	writel(reg_val, SID_PRCTL);

	while (readl(SID_PRCTL) & 0x1) {
	};

	reg_val &= ~((0xff << 8) | 0x3);
	writel(reg_val, SID_PRCTL);

	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
uint sid_read_key(uint key_index)
{
	uint reg_val;
	reg_val = readl(SID_PDKEY + key_index);

	return reg_val;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
void sid_read_oemid(void)
{
	uint chipid_index = EFUSE_OEM_ID;
	uint id_length = 2;
	uint i = 0;
	for (i = 0 ; i < id_length ; i++) {
		sid_read_key(chipid_index + i * 4);
	}
	return ;
}

int sid_write_oemid(void)
{
	uint chipid_index = EFUSE_OEM_ID;
	uint id_length = 2;
	uint i = 0;
	int ret = 0;

	for (i = 0 ; i < id_length ; i++) {
		ret = sid_program_key(chipid_index + i * 4, 0x00001234);
		if (ret < 0)
			return -1;
	}
	return 0;
}

/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
void sid_test(void)
{
	sid_read_oemid();
	sid_write_oemid();
	sid_read_oemid();
	return ;
}
