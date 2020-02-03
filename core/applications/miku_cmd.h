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

#endif