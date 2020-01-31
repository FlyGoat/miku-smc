/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-12-04     Jiaxun Yang  Initial version
 */

#ifndef __ASSEMBLY__
#define __ASSEMBLY__
#endif

#include <mips.h>

    .section ".text", "ax"
    .set noreorder

    .globl  rt_cpu_early_init
rt_cpu_early_init:
    /* Set Kseg0 Cache */
    mfc0	t0, CP0_CONFIG
    ori     	t0, t0, 7
    xori	t0, t0, 4
    mtc0	t0, CP0_CONFIG
    ehb

    la      t0, _etext
    la      t1, __data_start__
    la      t2, __data_end__

copy_data_loop:
    lw		v0, (t0)
    sw		v0, (t1)
    addiu   t0, 0x4
    addiu   t1, 0x4
    ble     t1, t2, copy_data_loop
    nop

    jr	ra