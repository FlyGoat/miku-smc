/* SPDX-License-Identifier: MIT OR Apache-2.0 */
/*
 * Copyright (C) 2019, 2020 Jiaxun Yang <jiaxun.yang@flygoat.com>
 */

#ifndef _MIKU_FEATURES_H__
#define _MIKU_FEATURES_H__

void miku_features_init(void);

#define CMD_GET_VERSION 0x1
/* Interface Version, input none, return version */

/* Features */
#define CMD_GET_FEATURES 0x2
/* Get features that SMC implemented, input index, output feature flags */
#define CMD_GET_ENABLED_FEATURES 0x3
/* Get currently enabled features, input index, output feature flags */
#define CMD_SET_ENABLED_FEATURES 0x4
/* Set features enabled state, input index and flags, output sucessfully enabled flags */

struct feature_args {
	u16 flags : 16;
	u8  index : 8;
};

enum miku_feature_set {
	MIKU_FEATURE_SET_GENERAL = 0x0,
	MIKU_FEATURE_SET_ADVANCED = 0x1,
	MIKU_FEATURE_SET_END = 0x2,
};

#define FEATURE_SET_GENERAL 0x0
#define FEATURE_DVFS ((1 << 1) | (1 << 0))
#define FEATURE_BOOST (1 << 2)
#define FEATURE_SENSORS (1 << 3)
#define FEATURE_FAN_CONTROL (1 << 4)

#endif