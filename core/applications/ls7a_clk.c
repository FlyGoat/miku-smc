/* SPDX-License-Identifier: MIT OR Apache-2.0 */
/*
 * Copyright (C) 2019, 2020 Jiaxun Yang <jiaxun.yang@flygoat.com>
 */

#include "miku.h"

#define REG_RTC_READ HWREG32(LS7A_RTC_RTCREAD)

rt_uint32_t time_stamp_ms(void)
{
	rt_uint32_t val;

	val = REG_RTC_READ;

	/* Round with 32.768Khz */
	return val / 33;
}