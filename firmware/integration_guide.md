# 固件集成指南

Miku SMC的代码与只读数据段放置于固件ROM中，从固件的6M处开始，其bss以及可写数据段由Miku自身在启动时置于系统主存。Stack和Heap也位于主存中，其使用的内存地址为固件保留地址，请确保Miku的内存不会与固件运行内存冲突，具体地址可参见`ls3-smc_rom.lds`中的定义。

## 启动部分
GS132核与主核共用ROM异常入口,所以需要在固件的NMI和通用异常入口判断是否为GS132以在启动时引导他。

请将以下代码放置到固件汇编代码最头部和exc_common入口。

```
#define GS132_CORE_ID 0x8000
mfc0	t0, $15, 0
andi	t0, t0, 0xe000
li	t2, GS132_CORE_ID
beq	t0, t2, go_132
nop
b	go_maincore:
nop
go_132:
li	t2, 0xbd600000
jr	t2
nop
go_maincore:
```

## 时钟与电压设置部分
请依照DVFS Policy设置基频时钟，以2200MHz Boost的Policy为例，基频为1650MHz，固件头部的MCSR CCFreq设置如下。

```
	/*set stable counter clk (CCFreq * CFM / CFD) = clk*/
	/*cfg mcsr addr = (((register num & 0x3fff) << 2)+ 0xfffe0000)*/
	li	t0, 0xfffe0010
	li	t1, 25000000 // 25MHz
	.word (0x32<<26 | 0x18 | 0x4<<6 | 0x1<<16 | T0<<21 | T1<<11)
	li	t0, 0xfffe0014
#define CORE_FREQ       1650 
	li	t1, (CORE_FREQ / 25) | (1 << 16) //1.6G [31:16]->CDF | [15:0]->CFM
	.word (0x32<<26 | 0x18 | 0x4<<6 | 0x1<<16 | T0<<21 | T1<<11)
#endif
```

PLL设置：L1_REFC=2, L1_LOOPC=33, L1_DIV=1, GS132_freqscale=3 /* 4/8 of Main PLL */

SRAM与电压按1.15V设置，偏压按P4N6设置。

其他按照原样。

## 固件引导部分
由于操作SPI Flash会导致GS132异常，所以我们需要在固件退出前，所有SPI Flash初始化后再指挥GS132初始化复位，对于PMON Bootloader，这个位置是 `void initstack (ac, av, addenv)` 函数中，加入以下代码：
```
(*(volatile unsigned int *)(0xbfe00420)) |= 0x100;
```

## 固件二进制部分
请将编译出的rtthread.bin拼接到固件Flash的6M处，中间空余部分可以用0xff填充。