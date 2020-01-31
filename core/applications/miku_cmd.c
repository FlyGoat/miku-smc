/* SPDX-License-Identifier: MIT OR Apache-2.0 */
/*
 * Copyright (C) 2019, 2020 Jiaxun Yang <jiaxun.yang@flygoat.com>
 */

#include "miku.h"

miku_func_t miku_cmd_func[MIKU_MAX_CMD];

#define MAILBOX_REG	HWREG32(SMC_MAILBOX_BASE)

rt_int8_t miku_handle_unexist_cmd(struct smc_message *msg)
{
	msg->arg = 0x0;
	MIKU_DBG("cmd: no such cmd\n");
	return MIKU_ENOCMD;
}

void miku_proc_cmd(void)
{
	struct smc_message msg;
	struct rt_thread *thread;
	thread = rt_thread_self();

	msg.value = MAILBOX_REG;

	/* We have no incoming cmd, wait until next cycle */
	if(msg.complete)
		return;

	if (msg.cmd >= MIKU_MAX_CMD) {
		msg.cmd = miku_handle_unexist_cmd(&msg);
	} else {
		msg.cmd = miku_cmd_func[msg.cmd](&msg);
	}

	msg.complete = 1;

	MAILBOX_REG = msg.value; /* Writeback to mailbox */

	return;
}

rt_err_t miku_cmd_install(rt_uint8_t id, miku_func_t func)
{
	if (!func || id >= MIKU_MAX_CMD)
		return RT_ERROR;
	
	miku_cmd_func[id] = func;
	return RT_EOK;
}

void miku_cmd_init(void)
{
	rt_int32_t i;
	rt_thread_t cmd_thread;

    	for (i=0; i < MIKU_MAX_CMD; i++)
        	miku_cmd_func[i] = (miku_func_t)miku_handle_unexist_cmd;
}