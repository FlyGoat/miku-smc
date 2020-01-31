/* SPDX-License-Identifier: MIT OR Apache-2.0 */
/*
 * Copyright (C) 2019, 2020 Jiaxun Yang <jiaxun.yang@flygoat.com>
 */

#include "miku.h"

#include "dvfs_policy_2200.h"

rt_uint16_t	target_shadow_freq[NUM_CORE];
rt_uint8_t	current_core_scale[NUM_CORE];
rt_uint32_t	core_scale_last_bump[NUM_CORE];

rt_uint8_t	current_pll_level;
rt_uint8_t	target_pll_level;
rt_uint16_t	current_vid;
rt_uint16_t	target_vid;
rt_uint32_t	last_bump_up;

static void core_set_scale(rt_uint8_t core_id, rt_uint8_t scale)
{
	if (scale > 0x7)
		scale = 0x7;

	if (scale == current_core_scale[core_id])
		return;

	MIKU_DBG("HW Set Core: %u, scale: %u", core_id, scale);
	current_core_scale[core_id] = scale;

	rt_uint32_t reg = HWREG32(LS3_CORE_SCALE_REG);

	reg &= ~(0x7 << (4 * core_id));
	reg |= scale << (4 * core_id);
	HWREG32(LS3_CORE_SCALE_REG) = reg;
}

static rt_uint16_t core_current_freq(rt_uint8_t core_id)
{
	return pll_to_freq(&pll_levels[current_pll_level]) * (current_core_scale[core_id] + 1) / 8;
}

rt_err_t miku_dvfs_fast_act(void)
{
	int i, j;
	rt_uint32_t time = time_stamp_ms();
	rt_uint32_t irq_level;
	irq_level = rt_hw_interrupt_disable();

	for(i = 0; i < NUM_CORE; i++) {
		rt_uint32_t target_scale = ((target_shadow_freq[i] * 80 / pll_to_freq(&pll_levels[current_pll_level]) + 5) / 10) - 1;

		if (target_scale >= current_core_scale[i])
			core_scale_last_bump[i] = time;
		else if ((time - core_scale_last_bump[i]) < core_scale_min)
			target_scale = core_scale_last_bump[i];


		if (target_scale > pll_levels[current_pll_level].highest_scale)
			target_scale = pll_levels[current_pll_level].highest_scale;
		
		if (target_scale < pll_levels[current_pll_level].lowest_scale)
			target_scale = pll_levels[current_pll_level].lowest_scale;

		core_set_scale(i, target_scale);
	}
	rt_hw_interrupt_enable(irq_level);

	return RT_EOK;
}

rt_err_t miku_judge_dvfs(void)
{
	rt_uint16_t highest_freq = 0;
	rt_uint8_t wanted_level = 0;
	rt_uint32_t time = time_stamp_ms();
	rt_int16_t temp = get_chiptemp();
	rt_uint32_t irq_level;
	int i;

	for(i = 0; i < NUM_CORE; i++) {
		if (target_shadow_freq[i] > highest_freq)
			highest_freq = target_shadow_freq[i];
	}

	wanted_level = 0;
	/* Now see what's the best level fit that freq */
	if (highest_freq > pll_to_freq(&pll_levels[NUM_PLL_LEVEL - 1]))
		wanted_level = NUM_PLL_LEVEL - 1;
	else {
		for(i = 0; i < NUM_PLL_LEVEL; i++) {
			if (pll_to_highest(&pll_levels[i]) >= highest_freq) {
				wanted_level = i;
				break;
			}
		}
	}

	if (wanted_level >= current_pll_level && current_pll_level != 0)
		last_bump_up = time;

	/* Check how long have we stay at current level to decide should we step down */
	if ((time - last_bump_up) < pll_levels[current_pll_level].min_time
		&& wanted_level < target_pll_level)
		wanted_level = target_pll_level;

	/* Thermal Throttling */
	while (pll_levels[wanted_level].max_temp < temp && wanted_level > 0)
		wanted_level--;

	irq_level = rt_hw_interrupt_disable();

	target_pll_level = wanted_level;
	target_vid = pll_levels[wanted_level].vid;

	rt_hw_interrupt_enable(irq_level);

	return RT_EOK;
}

rt_err_t miku_dvfs_action(void)
{
	rt_uint32_t irq_level;

	irq_level = rt_hw_interrupt_disable();

	/* Bump UP: VID First */
	if (current_vid < target_vid) {
		pmic_vctrl(target_vid);
		current_vid = target_vid;
	}

	if (current_pll_level != target_pll_level) {
		struct pll_level *action_level = &pll_levels[target_pll_level];

		MIKU_DBG("DVFS ACT: Level: %u", target_pll_level);
		ht_scale_sel(action_level->ht_scale);
		node_scale_sel(action_level->node_scale);

		if (pll_get_div() != action_level->div || pll_get_loopc() != action_level->loopc || pll_get_refc() != action_level->refc) {
			main_pll_sel(action_level->refc, action_level->loopc, action_level->div);
			MIKU_DBG("DVFS PLL: REFC: %u, LOOPC: %u, DIV: %u", action_level->refc, action_level->loopc, action_level->div);
		}
		current_pll_level = target_pll_level;
		stable_scale_sel(action_level->stable_scale);
		/* Resel Scale */
		miku_dvfs_fast_act();
	}

	/* Bump Down: PLL First */
	if (current_vid > target_vid) {
		pmic_vctrl(target_vid);
		current_vid = target_vid;
	}

	rt_hw_interrupt_enable(irq_level);

}

rt_uint8_t miku_get_freq_levels(struct smc_message *msg)
{
	rt_uint32_t argp = msg->arg;
	struct freq_level_args *arg = (struct freq_level_args*)&argp;

	arg->min_level = 0;
	arg->max_normal_level = BASEFREQ_SHADOW_LEVEL;
	arg->max_boost_level = SHADOW_LEVEL_NUM - 1;

	msg->value = argp;

	return MIKU_EOK;
}

rt_uint8_t miku_get_freq_info(struct smc_message *msg)
{
	rt_uint32_t argp = msg->arg;
	struct freq_info_args *arg = (struct freq_info_args*)&argp;

	if (arg->index == FREQ_INFO_INDEX_LEVEL_FREQ) {
		rt_uint8_t lvl = arg->info;
		rt_uint16_t freq;

		if (arg->info >= SHADOW_LEVEL_NUM)
			return MIKU_ECMDFAIL;

		freq = shadow_level_freq[lvl];
//		MIKU_DBG("DVFS: Get Level %u, FREQ: %u", lvl, freq);
		arg->info = freq;
	} else if (arg->index == FREQ_INFO_INDEX_CORE_FREQ) {
		if (arg->info >= NUM_CORE)
			return MIKU_ECMDFAIL;
		
		arg->info = core_current_freq(arg->info);
	} else
		return MIKU_ECMDFAIL;

	msg->value = argp;
	return MIKU_EOK;
	
}

rt_uint8_t miku_set_cpu_level(struct smc_message *msg)
{
	rt_uint32_t argp = msg->arg;
	struct freq_level_setting_args *arg = (struct freq_level_setting_args*)&argp;
	int i;
	
	if (!arg->cpumask)
		return MIKU_ECMDFAIL;

	if (arg->level >= SHADOW_LEVEL_NUM)
		return MIKU_ECMDFAIL;

	for (i = 0; i < NUM_CORE; i++)
	{
		if ((1 << i) & arg->cpumask) {
			uint16_t freq = shadow_level_freq[arg->level];
			target_shadow_freq[i] = freq;
//			MIKU_DBG("Setting Core %u, Shadow Freq: %u, Level %u", i, freq, arg->level);
		}
	}

	miku_dvfs_fast_act();
	msg->value = argp;
	return MIKU_EOK;
}

rt_err_t miku_enable_dvfs(void)
{
	miku_cmd_install(CMD_GET_FREQ_LEVELS, miku_get_freq_levels);
	miku_cmd_install(CMD_GET_FREQ_INFO, miku_get_freq_info);
	miku_cmd_install(CMD_SET_CPU_LEVEL, miku_set_cpu_level);
	MIKU_DBG("DVFS Enabled");
	return RT_EOK;
}

rt_err_t miku_dvfs_init(void)
{
	int i;
	rt_uint32_t time = time_stamp_ms();

	for (i = 0; i < NUM_CORE; i++) {
		target_shadow_freq[i] = BASEFREQ;
		current_core_scale[i] = 7;
		core_scale_last_bump[i] = time;
	}

	current_pll_level = 1;
	current_vid = 1250;
	last_bump_up = time;
}
