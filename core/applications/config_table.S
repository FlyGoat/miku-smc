/* SPDX-License-Identifier: MIT OR Apache-2.0 */
/*
 * Copyright (C) 2019, 2020 Jiaxun Yang <jiaxun.yang@flygoat.com>
 */


#ifndef __ASSEMBLY__
#define __ASSEMBLY__
#endif

#include "miku_config.h"

	.section ".config_table", "ax"
	.set noreorder
config_table_entry: /* RAM_BASE + 0x800 */
	.word(MIKU_MODEL_MAGIG) /* Magic Number + 0x0 */
	.word(MIKU_CURRENT_VERSION) /* Version + 0x4 */
	.word(0x1) /* Config Table Version + 0x8 */
	.word(25000000) /* CCFreq + 0xc */ /* FIXME: Coule be other vallues */
	.word(BASEFREQ / 25 | (1 << 16)) /* CFD and CFM + 0x10 */ /* FIXME: Coule be other vallues */
	.word(BOOT_DIV | (BOOT_LOOPC << 8) | (BOOT_REFC << 24)) /* PLL CFG + 0x14 */ /* [7:0]: DIV, [8:23]: LOOPC, [24:31]: REFC */
	.word(BOOT_VID) /* BOOT_VID mV + 0x18 */
	.word(BOOSTFREQ) /* BOOSTFREQ + 0x1c */
config_table_end:
	.ascii "MikuMikuMiku"