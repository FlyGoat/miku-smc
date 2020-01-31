/* SPDX-License-Identifier: MIT OR Apache-2.0 */
/*
 * Copyright (C) 2019, 2020 Jiaxun Yang <jiaxun.yang@flygoat.com>
 */


#include <rtthread.h>
#include "miku.h"

int main(int argc, char** argv)
{
	MIKU_DBG("Miki SMC\n");
	MIKU_DBG("Copyright (C) 2019, 2020 Jiaxun Yang <jiaxun.yang@flygoat.com>\n");
	MIKU_DBG("BRIDGE ID: %x\n", HWREG32(LS7A_MISC_BASE + 0x3ff8));

	/* 
	 * The main thread will be used to process CMD,
	 * We shouldn't kill it, otherwise childreen will die too. 
	 */
	miku_init();
	while(1) {
		miku_proc_cmd();
		/* Give up ticks to other tasks */
		rt_thread_yield();
	}
	return 0;
}
