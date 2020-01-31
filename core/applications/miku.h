/* SPDX-License-Identifier: MIT OR Apache-2.0 */
/*
 * Copyright (C) 2019, 2020 Jiaxun Yang <jiaxun.yang@flygoat.com>
 */

#ifndef _MIKU_H__
#define _MIKU_H__

#include <rtthread.h>
#include <mips.h>
#include <ls3-smc.h>
#include "miku_config.h"
#include "miku_cmd.h"
#include "miku_features.h"
#include "miku_sensors.h"
#include "miku_threads.h"
#include "miku_fan.h"
#include "miku_dvfs.h"

#ifdef MIKU_DEBUG
#define DBG_TAG               "Miku"
#define DBG_LVL               DBG_LOG
#include <rtdbg.h>
#define MIKU_DBG(...)	 LOG_D(__VA_ARGS__)
#else
#define MIKU_DBG(...)
#endif

rt_err_t miku_init(void);


#endif