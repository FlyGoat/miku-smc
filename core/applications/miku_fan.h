/* SPDX-License-Identifier: MIT OR Apache-2.0 */
/*
 * Copyright (C) 2019, 2020 Jiaxun Yang <jiaxun.yang@flygoat.com>
 */

#ifndef _MIKU_FAN_H__
#define _MIKU_FAN_H__

#include "miku.h"

struct level_table {
	rt_uint16_t low_bond;
	rt_uint16_t high_bond;
	rt_uint8_t level;
};

#define MIKU_FAN_NUM_TABLES 8

struct miku_fan
{
	rt_uint16_t flags;
	rt_uint16_t current_rpm;
	rt_uint8_t target_level;
	rt_uint8_t current_level;
	rt_uint8_t sourse_sensor;
	struct miku_fan_policy *policy;
};

struct miku_fan_policy
{
	int num_tables;
	struct level_table table[MIKU_FAN_NUM_TABLES];
};

enum miku_fan_ids {
#ifdef MIKU_FAN_HAVE_PACKAGE
	MIKU_FAN_ID_PACKAGE = 0x0,
#endif
	MIKU_FAN_ID_END,
};

struct fan_info_args {
	u16 val;
	u8  info_type : 4;
	u8  fan_id : 4;
};

extern rt_err_t miku_fan_init(void);
extern rt_err_t miku_enable_fan(void);
extern rt_err_t miku_fancontrol_decision(void);
extern rt_err_t miku_fancontrol_action(void);

#define FAN_LEVEL_MAX 255

/* Input sensor_id and info_type info, output none */
#define FAN_INFO_INDEX_RPM 0x0 /* Return RPM, can not set */
#define FAN_INFO_LEVEL 0x1 /* PWM Level, 0~255, only set with manual mode */
#define FAN_INFO_FLAGS 0xf /* Determine Mode */
#define FAN_FLAG_AUTO (1 << 0)
#define FAN_FLAG_MANUAL (1 << 1)


#endif