/* SPDX-License-Identifier: MIT OR Apache-2.0 */
/*
 * Copyright (C) 2019, 2020 Jiaxun Yang <jiaxun.yang@flygoat.com>
 */

#define BASEFREQ	1500
#define BOOT_VID	1150
#define BOOT_DIV	1
#define BOOT_LOOPC	15
#define BOOT_REFC	1
#define BOOSTFREQ	2000

#if defined(MIKU_INDVFS) && !defined(__ASSEMBLY__)
#define NUM_PLL_LEVEL	3

struct pll_level pll_levels[NUM_PLL_LEVEL] = 
{{/* Idle: Max 725MHz */
	.refc = BOOT_REFC,
	.loopc = BOOT_LOOPC,
	.div = BOOT_DIV,
	.vid = 1000, /* FIXME: Low down it later */
	.highest_scale = (4 - 1), /* 4 / 8 */
	.lowest_scale = 0, /* 1 / 8 */
	.stable_scale = (8 - 1), /* 8/8, 1500MHz */
	.node_scale = (4 - 1), /* 4 / 8 */
	.ht_scale = (3 - 1), /* 3 / 8 */
	.min_time = 5000, /* 5s */
	.max_temp = 120,
	.sram_val = 0x1f156f
},{/* Normal: Max 1500MHz */
	.refc = BOOT_REFC,
	.loopc = BOOT_LOOPC,
	.div = BOOT_DIV,
	.vid = BOOT_VID,
	.highest_scale = (8 - 1), /* 8 / 8 */
	.lowest_scale = (5 - 1), /* 5 / 8 */
	.stable_scale = (8 - 1), /* 8/8, 1500MHz */
	.node_scale = (8 - 1), /* 8 / 8 */
	.ht_scale = (4 - 1), /* 4 / 8 */
	.min_time = 5000, /* 5s */
	.max_temp = 100,
	.sram_val = 0x1e1020
},{/* Boost: Max 2000MHz */
	.refc = 1,
	.loopc = 20,
	.div = 1,
	.vid = 1400,
	.highest_scale = (8 - 1), /* 8 / 8 */
	.lowest_scale = (5 - 1), /* 5 / 8 */
	.stable_scale = (6 - 1), /* 6/8, 1500MHz */
	.node_scale = (8 - 1), /* 8 / 8 */
	.ht_scale = (4 - 1), /* 4 / 8 */
	.min_time = 5000, /* 5s */
	.max_temp = 80,
	.sram_val = 0x1e0040
}};

/* Shadow means what we tell kernel */
#define SHADOW_LEVEL_NUM	10
#define BASEFREQ_SHADOW_LEVEL	7
rt_uint16_t	shadow_level_freq[SHADOW_LEVEL_NUM] = {187, 375, 562, 750, 937, 1312, BASEFREQ, 1750, 2000};
rt_uint32_t	core_scale_min = 3000;
#endif