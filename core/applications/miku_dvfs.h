/* SPDX-License-Identifier: MIT OR Apache-2.0 */
/*
 * Copyright (C) 2019, 2020 Jiaxun Yang <jiaxun.yang@flygoat.com>
 */

#ifndef _MIKU_DVFS_H__
#define _MIKU_DVFS_H__

#define OSC_FREQ	100 /* MHz */
#define NUM_CORE	4

struct pll_level
{
	rt_uint8_t refc;
	rt_uint16_t loopc;
	rt_uint8_t div;
	rt_uint16_t vid; /* In mv */
	rt_uint8_t highest_scale;
	rt_uint8_t lowest_scale;
	rt_uint8_t stable_scale;
	rt_uint8_t node_scale;
	rt_uint8_t ht_scale;
	rt_uint32_t sram_val;
	rt_int8_t max_temp;
	rt_time_t min_time; /* Minimal time to stay before step down to a lower level */
};

static inline rt_uint16_t pll_to_freq(struct pll_level *pll)
{
	rt_uint32_t freq = OSC_FREQ * pll->loopc / pll->div / pll->refc;
	return (rt_uint16_t)freq;
}

static inline rt_uint16_t pll_to_highest(struct pll_level *pll)
{
	return pll_to_freq(pll) * (pll->highest_scale + 1) / 8;
}

rt_err_t miku_enable_dvfs(void);
rt_err_t miku_dvfs_init(void);
rt_err_t miku_dvfs_action(void);
rt_err_t miku_judge_dvfs(void);
void main_pll_sel(rt_uint8_t refc, rt_uint16_t loopc, rt_uint8_t div);
void stable_scale_sel(uint8_t scale);
void ht_scale_sel(uint8_t scale);
void node_scale_sel(uint8_t scale);
rt_uint8_t pll_get_refc(void);
rt_uint16_t pll_get_loopc(void);
rt_uint8_t pll_get_div(void);
void pmic_vctrl(u32 mv);
rt_uint32_t time_stamp_ms(void);

/* Freqscale Related */
#define CMD_SET_DVFS_POLICY 0x5
/* Input CPUNum, output frequency, in MHz? */
#define CMD_GET_FREQ_LEVELS 0x6
/* Input none, output levels */
struct freq_level_args {
	u8 min_level : 8;
	u8 max_normal_level : 8;
	u8 max_boost_level : 8;
};

#define CMD_GET_FREQ_INFO 0x7
/* Input index and info, output info */
#define CMD_SET_FREQ_INFO 0x8
/* Input index and info, output none */

#define FREQ_INFO_INDEX_LEVEL_FREQ 0x0 /* Freq in MHz? For each shadow level */
#define FREQ_INFO_INDEX_CORE_FREQ 0x1 /* Freq in MHz? Current frequency of each core.*/

struct freq_info_args {
	u16 info : 16;
	u8  index : 8;
};

#define CMD_SET_CPU_LEVEL 0x9
/* Input cpu mask and level, output none */
/*
 * Note: This command return as completed only means
 * SMC already knows the request, does not means the
 * CPU freqency have changed. SMC should ensure constant
 * counter frequency unchanged.
 */
struct freq_level_setting_args {
	u16 cpumask : 16;
	u8  level : 8;
};

#endif