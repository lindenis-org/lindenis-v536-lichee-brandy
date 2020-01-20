/*
 * (C) Copyright 2017-2018
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * wangwei <wangwei@allwinnertech.com>
 *
 * Configuration settings for the Allwinner sunxi series of boards.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <sys_partition.h>
#include <sys_config_old.h>
#include <sunxi_board.h>
#include <asm/io.h>
#include <smc.h>

int get_chipid(char* chipid)
{
	u32 sunxi_soc_chipid[4];

	sunxi_soc_chipid[0] = smc_readl(SUNXI_SID_BASE + 0x200);
	sunxi_soc_chipid[1] = smc_readl(SUNXI_SID_BASE + 0x200 + 0x4);
	sunxi_soc_chipid[2] = smc_readl(SUNXI_SID_BASE + 0x200 + 0x8);
	sunxi_soc_chipid[3] = smc_readl(SUNXI_SID_BASE + 0x200 + 0xc);

	sprintf(chipid , "%08x%08x%08x%08x",
		sunxi_soc_chipid[0], sunxi_soc_chipid[1], sunxi_soc_chipid[2], sunxi_soc_chipid[3]);
	return 0;
}

int sunxi_set_sid_num(void)
{
	char chipid[256] = {0};
	char* p = NULL;

	p = getenv("chipid");
	if (p != NULL)
		return 0;

	get_chipid(chipid);

	pr_msg("chipid num is: %s\n", chipid);
	if (setenv("chipid", chipid))
		pr_error("error:set env chipid fail\n");

	return 0;
}
