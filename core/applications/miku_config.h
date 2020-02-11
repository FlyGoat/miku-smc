/* SPDX-License-Identifier: MIT OR Apache-2.0 */
/*
 * Copyright (C) 2019, 2020 Jiaxun Yang <jiaxun.yang@flygoat.com>
 */

#ifndef _MIKU_CONFIG_H__
#define _MIKU_CONFIG_H__


#define MIKU_MODEL_MAGIG	0xbcda1901
#define MIKU_CURRENT_VERSION 0x1

#define MIKU_DEBUG	1
#define MIKU_HAVE_SENSORS	1
#define MIKU_SENSORS_HAVE_PACKAGE	1

#define MIKU_HAVE_FANCONTROL	1
#define MIKU_FAN_HAVE_PACKAGE	1
#define MIKU_HAVE_DVFS		1

#include "dvfs_policy_2200.h"
//#include "dvfs_policy_2000.h"

#endif