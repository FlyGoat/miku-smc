/* SPDX-License-Identifier: MIT OR Apache-2.0 */
/*
 * Copyright (C) 2019, 2020 Jiaxun Yang <jiaxun.yang@flygoat.com>
 */

#ifndef _MIKU_CMD_H__
#define _MIKU_CMD_H__

struct smc_message {
	union {
		u32 value;
		struct {
			u32 arg : 24;
			u8  cmd : 7; /* Also act as return */
			u8  complete : 1;
		};
	};
};

typedef rt_uint8_t (* miku_func_t)(struct smc_message *msg);

void miku_cmd_init(void);
void miku_proc_cmd(void);
rt_err_t miku_cmd_install(rt_uint8_t id, miku_func_t func);

#define MIKU_MAX_CMD	0x50

#define MIKU_EOK	0x0
/* Please prevent confiliction between command and error */
#define MIKU_ECMDFAIL	0x70
/* To be filled */
#define MIKU_ENOCMD	0x7f

/* Below is command list */
#define CMD_GET_VERSION 0x1
/* Interface Version, input none, return version */
#define CMD_GET_FEATURES 0x2
/* Get features that SMC implemented, input index, output feature flags */
#define CMD_GET_ENABLED_FEATURES 0x3
/* Get currently enabled features, input index, output feature flags */
#define CMD_SET_ENABLED_FEATURES 0x4
/* Set features enabled state, input index and flags, output sucessfully enabled flags */

/* TEMP Sensors */
#define CMD_GET_SENSOR_NUM 0x10
/* Input none, output Number of sensors in u4 */

#define CMD_GET_SENSOR_STATUS 0x11
/* Input sensor_id and info_type, output info */

/* Fan Control */
#define CMD_GET_FAN_NUM 0x12
/* Input none, output Number of fans in u4 */

#define CMD_GET_FAN_INFO 0x13
/* Input sensor_id and info_type, output info */
#define CMD_SET_FAN_INFO 0x14

#endif