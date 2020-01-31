/* SPDX-License-Identifier: MIT OR Apache-2.0 */
/*
 * Copyright (C) 2019, 2020 Jiaxun Yang <jiaxun.yang@flygoat.com>
 */

#include "miku.h"

static struct miku_fan miku_fans[MIKU_FAN_ID_END];

static struct miku_fan_policy cpufan_policy =
{
	.num_tables = 5,
	.table[0] = {
		/* Keep Fan in low speed */
		.low_bond = -32768,
		.high_bond = 15,
		.level = 30,
	},
	.table[1] = {
		.low_bond = 15,
		.high_bond = 40,
		.level = 100,
	},
	.table[2] = {
		.low_bond = 40,
		.high_bond = 50,
		.level = 150,
	},
	.table[3] = {
		.low_bond = 50,
		.high_bond = 70,
		.level = 175,
	},
	.table[4] = {
		.low_bond = 70,
		.high_bond = 32767,
		.level = 255,
	},
};

static rt_uint8_t miku_cmd_get_fan_num(struct smc_message *msg)
{
	msg->arg = MIKU_FAN_ID_END;
	return RT_EOK;
}

static rt_uint8_t miku_cmd_get_fan_info(struct smc_message *msg)
{
	rt_uint32_t argp = msg->arg;
	struct  fan_info_args *arg = (struct fan_info_args*)&argp;

	MIKU_DBG("Fan: get fan info id: %x, type: %x\n", arg->fan_id, arg->info_type);
	if (arg->fan_id >= MIKU_FAN_ID_END)
		return MIKU_ECMDFAIL;
	
	if (arg->info_type == FAN_INFO_FLAGS) {
		arg->val = miku_fans[arg->fan_id].flags;
	} else if (arg->info_type == FAN_INFO_INDEX_RPM) {
		arg->val = miku_fans[arg->fan_id].current_rpm;
	} else if (arg->info_type == FAN_INFO_LEVEL) {
		arg->val = miku_fans[arg->fan_id].current_level;
	} else {
		return MIKU_ECMDFAIL;
	}

	msg->value = argp;
	return MIKU_EOK;
}

static rt_err_t miku_fan_set_flags(rt_uint8_t id, rt_uint16_t flags)
{
	if (flags & FAN_FLAG_MANUAL) {
		miku_fans[id].flags &= ~FAN_FLAG_AUTO;
		miku_fans[id].flags |= FAN_FLAG_MANUAL;
		/* Fail safe */
		miku_fans[id].target_level = FAN_LEVEL_MAX;
	} else if (flags & FAN_FLAG_AUTO) {
		miku_fans[id].flags &= ~FAN_FLAG_MANUAL;
		miku_fans[id].flags |= FAN_FLAG_AUTO;
		/* Fail safe */
		miku_fans[id].target_level = FAN_LEVEL_MAX;
	} else {
		return RT_ERROR;
	}

	return RT_EOK;
}

static rt_uint8_t miku_cmd_set_fan_info(struct smc_message *msg)
{
	rt_uint32_t argp = msg->arg;
	struct  fan_info_args *arg = (struct fan_info_args*)&argp;

	MIKU_DBG("Fan: set fan info id: %x, type: %x\n", arg->fan_id, arg->info_type);
	if (arg->fan_id >= MIKU_FAN_ID_END)
		return MIKU_ECMDFAIL;
	
	if (arg->info_type == FAN_INFO_FLAGS) {
		if (miku_fan_set_flags(arg->fan_id, arg->val) != RT_EOK)
			return MIKU_ECMDFAIL;
	} else if (arg->info_type == FAN_INFO_LEVEL) {
		if (!(miku_fans[arg->fan_id].flags & FAN_FLAG_MANUAL))
			return MIKU_ECMDFAIL;
		miku_fans[arg->fan_id].target_level = arg->val;
		MIKU_DBG("Fan: set target level: %d\n", miku_fans[arg->fan_id].target_level);
	} else {
		return MIKU_ECMDFAIL;
	}

	msg->value = argp;
	return MIKU_EOK;
}

rt_err_t miku_enable_fan(void)
{
	miku_cmd_install(CMD_GET_FAN_NUM, miku_cmd_get_fan_num);
	miku_cmd_install(CMD_GET_FAN_INFO, miku_cmd_get_fan_info);
	miku_cmd_install(CMD_SET_FAN_INFO, miku_cmd_set_fan_info);
	return RT_EOK;
}

static void miku_ls7a_pwm_set_level(u8 level)
{
	MIKU_DBG("ls7a set fan level %d\n", level);
	writel(255, LS7A_PWM0_FULL);
	writel(255 - level, LS7A_PWM0_LOW);
	writel(255, LS7A_PWM1_FULL);
	writel(255 - level, LS7A_PWM1_LOW);
	writel(255, LS7A_PWM2_FULL);
	writel(255 - level, LS7A_PWM2_LOW);
	writel(255, LS7A_PWM3_FULL);
	writel(255 - level, LS7A_PWM3_LOW);
}

static rt_uint8_t miku_fan_judge_level(rt_int16_t temp, struct miku_fan_policy *policy)
{
	int i;
	for (i = 0; i < policy->num_tables; i++)
	{
		if (policy->table[i].high_bond >= temp &&
			policy->table[i].low_bond <= temp)
			return policy->table[i].level;
	}
	/* Not Matched, Return Max */
	return 255;
}

rt_err_t miku_fancontrol_decision(void)
{
	int i;
	for (i = 0; i < MIKU_FAN_ID_END; i++)
	{
		/* Do not judge at manual mode */
		if (miku_fans[i].flags & FAN_FLAG_MANUAL)
			continue;

		rt_int16_t temp = miku_sensor_get_temp(miku_fans[i].sourse_sensor);
		miku_fans[i].target_level = miku_fan_judge_level(temp, miku_fans[i].policy);
	}

	return RT_EOK;
}

rt_err_t miku_fancontrol_action(void)
{
	/* FIXME: Apply a generic method */
	if (miku_fans[MIKU_FAN_ID_PACKAGE].current_level != miku_fans[MIKU_FAN_ID_PACKAGE].target_level) {
		miku_ls7a_pwm_set_level(miku_fans[MIKU_FAN_ID_PACKAGE].target_level);
		miku_fans[MIKU_FAN_ID_PACKAGE].current_level = miku_fans[MIKU_FAN_ID_PACKAGE].target_level;
	}
}

rt_err_t miku_fan_init(void)
{
	/* FIXME: Apply a generic method */
	miku_fans[MIKU_FAN_ID_PACKAGE].target_level = 255;
	miku_fans[MIKU_FAN_ID_PACKAGE].flags = FAN_FLAG_AUTO;
	miku_fans[MIKU_FAN_ID_PACKAGE].sourse_sensor = MIKU_SENSOR_ID_PACKAGE;
	miku_fans[MIKU_FAN_ID_PACKAGE].policy = &cpufan_policy;
}