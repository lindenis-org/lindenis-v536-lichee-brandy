/*
 * (C) Copyright 2007-2013
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/timer.h>
#include <asm/arch/ccmu.h>
#include <asm/arch/clock.h>
#include <asm/arch/sid.h>
#include <asm/arch/platform.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/key.h>
#include <asm/arch/dma.h>

#include <boot_type.h>
#include <sys_partition.h>
#include <sys_config.h>
#include <power/sunxi/pmu.h>
#include <smc.h>
#include <sunxi_board.h>
#include <fdt_support.h>
#include <sys_config_old.h>
#include "asm/arch/rtc_region.h"

/* The sunxi internal brom will try to loader external bootloader
 * from mmc0, nannd flash, mmc2.
 * We check where we boot from by checking the config
 * of the gpio pin.
 */
DECLARE_GLOBAL_DATA_PTR;

extern void power_limit_init(void);
extern s32 axp22_usb_vbus_output(int hight);

#ifdef CONFIG_SUNXI_MODULE_AXP
int power_source_init(void)
{
	int pll_cpux;
	int cpu_vol;
	uint32_t dcdc_vol = 0;
	int axp_exist = 0;
	int nodeoffset;

	//PMU_SUPPLY_DCDC2 is for cpua
	nodeoffset =  fdt_path_offset(working_fdt,FDT_PATH_POWER_SPLY);
	if(nodeoffset >=0)
	{
		fdt_getprop_u32(working_fdt, nodeoffset, "dcdc2_vol", &dcdc_vol);
	}
	if(!dcdc_vol)
	{
		cpu_vol = 900;
	}
	else
	{
		cpu_vol = dcdc_vol%10000;
	}
	axp_exist =  axp_probe();
	if(axp_exist)
	{
		axp_probe_factory_mode();
		if(!axp_probe_power_supply_condition())
		{
			//PMU_SUPPLY_DCDC2 is for cpua
			if(!axp_set_supply_status(0, PMU_SUPPLY_DCDC2, cpu_vol, -1))
			{
				tick_printf("PMU: dcdc2 %d\n", cpu_vol);
				sunxi_clock_set_corepll(uboot_spare_head.boot_data.run_clock);
			}
			else
			{
				printf("axp_set_dcdc2 fail\n");
			}
		}
		else
		{
			printf("axp_probe_power_supply_condition error\n");
		}
	}
	else
	{
		printf("axp_probe error\n");
	}

	pll_cpux = sunxi_clock_get_corepll();
	tick_printf("PMU: cpux %d Mhz,AXI=%d Mhz\n", pll_cpux,sunxi_clock_get_axi());
	printf("PLL6=%d Mhz,AHB1=%d Mhz, APB1=%dMhz  MBus=%dMhz\n", sunxi_clock_get_pll6(),
		sunxi_clock_get_ahb(),
		sunxi_clock_get_apb(),
		sunxi_clock_get_mbus());

	if(axp_exist)
	{
		axp_set_charge_vol_limit();
		axp_set_all_limit();
		axp_set_hardware_poweron_vol();
		axp_set_power_supply_output();
		//power_config_gpio_bias();
		power_limit_init();
	}

	return 0;
}
#else
int power_source_init(void)
{
	return 0;
}
#endif
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
int sunxi_probe_securemode(void)
{
	gd->securemode = SUNXI_NORMAL_MODE;
	gd->bootfile_mode = SUNXI_BOOT_FILE_PKG;

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
int sunxi_set_secure_mode(void)
{
	return 0;
}

s32 axp_usb_vbus_output(void)
{
	int vbus_gpio,ret;
	ret = script_parser_fetch("platform", "boot_usb0_drv_vbus_gpio",
							(int *)&vbus_gpio, 1);
	if(ret < 0) {
		debug("%s:get boot_usb0_drv_vbus_gpio error\n",__func__);
		vbus_gpio = 0;
	}

	if (axp22_usb_vbus_output(vbus_gpio) > 0)
		return 1;

	return 0;
}

int sunxi_set_bootmode_flag(u8 flag)
{
	volatile uint reg_val;
	do {
		writel(flag, RTC_DATA_HOLD_REG_BASE + SUNXI_RTC_GPREG_NUM * 0x4);
		reg_val = readl(RTC_DATA_HOLD_REG_BASE + SUNXI_RTC_GPREG_NUM * 0x4);
	} while ((reg_val & 0xff) != flag);
	return 0;
}

int sunxi_get_bootmode_flag(void)
{
	uint fel_flag;
	/* operation should be same with kernel write rtc */
	fel_flag = readl(RTC_DATA_HOLD_REG_BASE + SUNXI_RTC_GPREG_NUM*0x4);
	return fel_flag;
}
