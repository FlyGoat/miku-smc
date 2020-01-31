/* SPDX-License-Identifier: MIT OR Apache-2.0 */
/*
 * Copyright (C) 2019, 2020 Jiaxun Yang <jiaxun.yang@flygoat.com>
 */

#include "miku.h"

#define NUM_PLL_LEVEL	3

struct pll_level pll_levels[NUM_PLL_LEVEL] = 
{{/* Idle: Max 825MHz */
	.refc = 2,
	.loopc = 33,
	.div = 1,
	.vid = 1150, /* FIXME: Low down it later */
	.highest_scale = (4 - 1), /* 4 / 8 */
	.lowest_scale = 0, /* 1 / 8 */
	.stable_scale = (8 - 1), /* 8/8, 1650MHz */
	.node_scale = (4 - 1), /* 4 / 8 */
	.ht_scale = (3 - 1), /* 3 / 8 */
	.min_time = 5000 /* 5s */
},{/* Normal: Max 1650MHz */
	.refc = 2,
	.loopc = 33,
	.div = 1,
	.vid = 1150,
	.highest_scale = (8 - 1), /* 8 / 8 */
	.lowest_scale = (5 - 1), /* 5 / 8 */
	.stable_scale = (8 - 1), /* 8/8, 1650MHz */
	.node_scale = (8 - 1), /* 8 / 8 */
	.ht_scale = (4 - 1), /* 4 / 8 */
	.min_time = 5000 /* 5s */
},{/* Boost: Max 2200MHz */
	.refc = 1,
	.loopc = 22,
	.div = 1,
	.vid = 1450,
	.highest_scale = (8 - 1), /* 8 / 8 */
	.lowest_scale = (5 - 1), /* 5 / 8 */
	.stable_scale = (6 - 1), /* 6/8, 1650MHz */
	.node_scale = (8 - 1), /* 8 / 8 */
	.ht_scale = (4 - 1), /* 4 / 8 */
	.min_time = 5000 /* 5s */
}};

/* Shadow means what we tell kernel */
#define SHADOW_LEVEL_NUM	10
#define BASEFREQ_SHADOW_LEVEL	7
#define BASEFREQ	1650
rt_uint16_t	shadow_level_freq[SHADOW_LEVEL_NUM] = {200, 410, 618, 825, 1030, 1237, 1443, BASEFREQ, 1925, 2200};
rt_uint32_t	core_scale_min = 3000;
