/* SPDX-License-Identifier: MIT OR Apache-2.0 */
/*
 * Copyright (C) 2019, 2020 Jiaxun Yang <jiaxun.yang@flygoat.com>
 */

#include "miku.h"

rt_err_t miku_init(void)
{
	miku_cmd_init();
	miku_features_init();
	miku_sensors_init();
	miku_fan_init();
	miku_dvfs_init();
	miku_threads_init();
}