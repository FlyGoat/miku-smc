/* SPDX-License-Identifier: MIT OR Apache-2.0 */
/*
 * Copyright (C) 2019, 2020 Jiaxun Yang <jiaxun.yang@flygoat.com>
 */

#ifndef _MIKU_FEATURES_H__
#define _MIKU_FEATURES_H__

#include <mips.h>

void miku_features_init(void);

enum miku_feature_set {
	MIKU_FEATURE_SET_GENERAL = 0x0,
	MIKU_FEATURE_SET_ADVANCED = 0x1,
	MIKU_FEATURE_SET_END = 0x2,
};

struct feature_args {
	u16 flags : 16;
	u8  index : 8;
};

#define FEATURE_SET_GENERAL 0x0
#define FEATURE_DVFS ((1 << 1) | (1 << 0))
#define FEATURE_BOOST (1 << 2)
#define FEATURE_SENSORS (1 << 3)
#define FEATURE_FAN_CONTROL (1 << 4)

#endif