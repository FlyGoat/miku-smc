/* SPDX-License-Identifier: MIT OR Apache-2.0 */
/*
 * Copyright (C) 2019, 2020 Jiaxun Yang <jiaxun.yang@flygoat.com>
 */

#include "miku.h"

rt_thread_t sampling;
rt_thread_t decision;
rt_thread_t action;

void miku_sampling_thread(void *parameter)
{
	while (1)
	{
		/* This thread used to update datas from sensors */
		rt_thread_yield();
	}
}

void miku_decision_thread(void *parameter)
{
	while(1)
	{
		/* This thread is used to make decisions of target values */
		miku_fancontrol_decision();
		miku_judge_dvfs();
		/* This thread is used to do adjustments after decistions made */	
		miku_fancontrol_action();
		miku_dvfs_action();
		rt_thread_yield();
		rt_thread_yield();
	}
}



void miku_main()
{
	/* This should be called after miku_init in main */
	while(1) {
		miku_proc_cmd();
		rt_thread_yield();
	}
}

rt_err_t miku_threads_init()
{
	sampling = rt_thread_create("miku_sampling", miku_sampling_thread, RT_NULL, MIKU_DEFAULT_STACK_SIZE,
					RT_MAIN_THREAD_PRIORITY, 20);
	if(sampling != RT_NULL)
		rt_thread_startup(sampling);
	else
		return RT_ERROR;
	

	decision = rt_thread_create("miku_decision", miku_decision_thread, RT_NULL, MIKU_DEFAULT_STACK_SIZE,
					RT_MAIN_THREAD_PRIORITY, 20);
	if(decision != RT_NULL)
		rt_thread_startup(decision);
	else
		return RT_ERROR;

	return RT_EOK;
}