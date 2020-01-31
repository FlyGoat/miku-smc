/* SPDX-License-Identifier: MIT OR Apache-2.0 */
/*
 * Copyright (C) 2019, 2020 Jiaxun Yang <jiaxun.yang@flygoat.com>
 */

#include <rtthread.h>
#include <ls3-smc.h>

#define GENMASK(h,l)    	(((rt_uint32_t)(1UL << ((h)-(l)+1))-1)<<(l))

#define MAIN_PLL_REG_LO	REG32(MAIN_PLL_BASE)
#define MAIN_PLL_REG_HI	REG32(MAIN_PLL_BASE + 0x4)


/* Low Regs */
#define SEL_PLL_NODE_B	(1 << 0)
#define SOFT_SEL_PLL_B	(1 << 2)
#define BYPASS_B	(1 << 3)
#define LOCKEN_B	(1 << 7)
#define LOCKC_F		10
#define LOCKED_B	(1 << 16)
#define PD_B		(1 << 19)
#define REFC_F		26
#define REFC_MASK	GENMASK(31, 26)

#define LOCKC_VAL	0x3

/* High Regs */
#define LOOPC_F	0
#define LOOPC_MASK	GENMASK(8, 0)
#define DIV_F	10
#define DIV_MASK	GENMASK(15, 10)


#define FUNC_CFG_LOW	REG32(0xbfe00180)
#define HT0_SCALE_F	24
#define HT0_SCALE_MASK	GENMASK(26, 24)
#define HT1_SCALE_F	28
#define HT1_SCALE_MASK	GENMASK(30, 28)

#define FUNC_CFG_HI	REG32(0xbfe00180 + 0x4)
#define NODE_SCALE_F	8
#define NODE_SCALE_MASK	GENMASK(10, 8)

#define MISC_CFG_HI	REG32(0xbfe00420 + 0x4)
#define STABLE_SCALE_F	12
#define STABLE_SCALE_MASK	GENMASK(14, 12)

void main_pll_sel(rt_uint8_t refc, rt_uint16_t loopc, rt_uint8_t div)
{
	rt_uint32_t low, hi;
	low = MAIN_PLL_REG_LO;
	hi = MAIN_PLL_REG_HI;

	/* Clear SEL Bits */
	low &= ~(SEL_PLL_NODE_B | SOFT_SEL_PLL_B);
	MAIN_PLL_REG_LO = low;
//	rt_kprintf("step1: clr sel\n");
	low |= PD_B;
//	rt_kprintf("step2: pd\n");

	/* Write Once */
	MAIN_PLL_REG_LO = low;

	MAIN_PLL_REG_HI = (loopc << LOOPC_F) | (div << DIV_F);
	low = (refc << REFC_F) | (LOCKC_VAL << LOCKC_F) | LOCKEN_B;
	MAIN_PLL_REG_LO = low;
	low |= SOFT_SEL_PLL_B;
	MAIN_PLL_REG_LO = low;
//	rt_kprintf("step3: set val\n");

	/* Wait until PLL Locked */
	while(!(MAIN_PLL_REG_LO & LOCKED_B)){
//		rt_kprintf("waiting val: x\n", MAIN_PLL_REG_LO);
	}
//	rt_kprintf("step4: wait lock\n");

	MAIN_PLL_REG_LO |= SEL_PLL_NODE_B;
//	rt_kprintf("step5: sel pll\n");
}

rt_uint8_t pll_get_refc(void)
{
	rt_uint32_t lo = MAIN_PLL_REG_LO;

	lo &= REFC_MASK;
	lo = lo >> REFC_F;

	return lo;
}

rt_uint16_t pll_get_loopc(void)
{
	rt_uint32_t hi = MAIN_PLL_REG_HI;

	hi &= LOOPC_MASK;
	hi = hi >> REFC_F;

	return hi;
}

rt_uint8_t pll_get_div(void)
{
	rt_uint32_t hi = MAIN_PLL_REG_HI;

	hi &= DIV_MASK;
	hi = hi >> DIV_F;

	return hi;
}

void stable_scale_sel(uint8_t scale)
{
	rt_uint32_t hi = MISC_CFG_HI;
	hi &= ~STABLE_SCALE_MASK;
	hi |= scale << STABLE_SCALE_F;
	MISC_CFG_HI = hi;
}

void ht_scale_sel(uint8_t scale)
{
	rt_uint32_t low = FUNC_CFG_LOW;
	low &= ~HT0_SCALE_MASK;
	low |= scale << HT0_SCALE_F;
	low &= ~HT1_SCALE_MASK;
	low |= scale << HT1_SCALE_F;
	FUNC_CFG_LOW = low;
}

void node_scale_sel(uint8_t scale)
{
	rt_uint32_t hi = FUNC_CFG_HI;
	hi &= ~NODE_SCALE_MASK;
	hi |= scale << NODE_SCALE_F;
	FUNC_CFG_HI = hi;
}