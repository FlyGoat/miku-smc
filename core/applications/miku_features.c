/* SPDX-License-Identifier: MIT OR Apache-2.0 */
/*
 * Copyright (C) 2019, 2020 Jiaxun Yang <jiaxun.yang@flygoat.com>
 */

#include "miku.h"

#define MIKU_CURRENT_VERSION 0x1

static rt_uint32_t implemented_feature[MIKU_FEATURE_SET_END];
static rt_uint32_t enabled_feature[MIKU_FEATURE_SET_END];

rt_uint8_t miku_get_version(struct smc_message *msg)
{
	MIKU_DBG("miku_get_version\n");
	msg->arg = MIKU_CURRENT_VERSION;
	return MIKU_EOK;
}

rt_uint8_t miku_enable_feature(struct smc_message *msg)
{
	/* Ugly method to recast arg... But I have no choice */
	rt_uint32_t argp = msg->arg;
	struct feature_args *arg = (struct feature_args*)&argp;

	if(arg->index >= MIKU_FEATURE_SET_END)
		return MIKU_ECMDFAIL;

	if(arg->index == MIKU_FEATURE_SET_GENERAL) {
#ifdef MIKU_HAVE_SENSORS
		if (arg->flags & FEATURE_SENSORS) {
			if (miku_enable_sensors() == RT_EOK)
				enabled_feature[MIKU_FEATURE_SET_GENERAL] |= FEATURE_SENSORS;
		}
#endif
#ifdef MIKU_HAVE_FANCONTROL
		if (arg->flags & FEATURE_FAN_CONTROL) {
			if (miku_enable_fan() == RT_EOK)
				enabled_feature[MIKU_FEATURE_SET_GENERAL] |= FEATURE_FAN_CONTROL;
		}
#endif
#ifdef MIKU_HAVE_DVFS
		if (arg->flags & FEATURE_DVFS) {
			if (miku_enable_dvfs() == RT_EOK)
				enabled_feature[MIKU_FEATURE_SET_GENERAL] |= FEATURE_DVFS;
		}
		if (arg->flags & FEATURE_BOOST) {
				enabled_feature[MIKU_FEATURE_SET_GENERAL] |= FEATURE_BOOST;
		}
#endif
	} else {
		return MIKU_ECMDFAIL;
	}

	msg->arg = enabled_feature[arg->index];

	if (!(enabled_feature[arg->index] & arg->flags))
		return MIKU_ECMDFAIL;

	return MIKU_EOK;
}

rt_uint8_t miku_get_enabled_feature(struct smc_message *msg)
{
	/* Ugly method to recast arg... But I have no choice */
	rt_uint32_t argp = msg->arg;
	struct feature_args *arg = (struct feature_args*)&argp;

	if(arg->index >= MIKU_FEATURE_SET_END)
		return MIKU_ECMDFAIL;
	msg->arg = enabled_feature[arg->index];

	return MIKU_EOK;
}

rt_uint8_t miku_get_feature(struct smc_message *msg)
{
	/* Ugly method to recast arg... But I have no choice */
	rt_uint32_t argp = msg->arg;
	struct feature_args *arg = (struct feature_args*)&argp;

	if(arg->index >= MIKU_FEATURE_SET_END)
		return MIKU_ECMDFAIL;
	msg->arg = implemented_feature[arg->index];

	return MIKU_EOK;
}

void miku_features_init(void) {
	int i;
	for (i = 0; i < MIKU_FEATURE_SET_END; i++)
		enabled_feature[i] = 0x0;

#ifdef MIKU_HAVE_SENSORS
	implemented_feature[MIKU_FEATURE_SET_GENERAL] |= FEATURE_SENSORS;
#endif
#ifdef MIKU_HAVE_FANCONTROL
	implemented_feature[MIKU_FEATURE_SET_GENERAL] |= FEATURE_FAN_CONTROL;
#endif
#ifdef MIKU_HAVE_DVFS
	implemented_feature[MIKU_FEATURE_SET_GENERAL] |= FEATURE_DVFS;
	implemented_feature[MIKU_FEATURE_SET_GENERAL] |= FEATURE_BOOST;
#endif

	miku_cmd_install(CMD_GET_VERSION, miku_get_version);
	miku_cmd_install(CMD_GET_FEATURES, miku_get_feature);
	miku_cmd_install(CMD_GET_ENABLED_FEATURES, miku_get_enabled_feature);
	miku_cmd_install(CMD_SET_ENABLED_FEATURES, miku_enable_feature);
}