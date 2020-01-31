/* SPDX-License-Identifier: MIT OR Apache-2.0 */
/*
 * Copyright (C) 2019, 2020 Jiaxun Yang <jiaxun.yang@flygoat.com>
 */

#ifndef _MIKU_THREADS_H__
#define _MIKU_THREADS_H__

#include "miku.h"

#define MIKU_DEFAULT_STACK_SIZE	2048

void miku_main();
rt_err_t miku_threads_init();

#endif