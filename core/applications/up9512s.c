/* SPDX-License-Identifier: MIT OR Apache-2.0 */
/*
 * Copyright (C) 2019, 2020 Jiaxun Yang <jiaxun.yang@flygoat.com>
 */

#include "miku.h"

#define PRER_LO_REG	0x0
#define PRER_HI_REG	0x1
#define CTR_REG    	0x2
#define TXR_REG    	0x3
#define RXR_REG    	0x3
#define CR_REG     	0x4
#define SR_REG     	0x4
#define SLV_CTRL_REG	0x7

#define CR_START					0x80
#define CR_STOP						0x40
#define CR_READ						0x20
#define CR_WRITE					0x10
#define CR_ACK						0x8
#define CR_IACK						0x1

#define SR_NOACK					0x80
#define SR_BUSY						0x40
#define SR_AL						0x20
#define SR_TIP						0x2

#define	UPI9212S_ADDR0	        0x4a    //3A I2C0

void  *i2c_base_addr;
unsigned char word_offset = 0;

static void ls_i2c_stop(void)
{
	do {
		writeb(CR_STOP, i2c_base_addr + CR_REG);
		readb(i2c_base_addr + SR_REG);
	} while (readb(i2c_base_addr + SR_REG) & SR_BUSY);
}

void ls_i2c_init(void)
{
	u8 val;
	MIKU_DBG("I2C BASE 0x%lx \n", (unsigned long)i2c_base_addr);
	val = readb(i2c_base_addr + CTR_REG);
	val &= ~0x80;
	writeb(val, i2c_base_addr + CTR_REG);
	writeb(0x71, i2c_base_addr + PRER_LO_REG);
	writeb(0x1, i2c_base_addr + PRER_HI_REG);
	val = readb(i2c_base_addr + CTR_REG);
	val |=  0x80;
	writeb(val, i2c_base_addr + CTR_REG);
}

static int ls_i2c_tx_byte(unsigned char data, unsigned char opt)
{
	int times = 1000000;
	writeb(data, i2c_base_addr + TXR_REG);
	writeb(opt, i2c_base_addr + CR_REG);
	while ((readb(i2c_base_addr + SR_REG) & SR_TIP) && times--);
	if (times < 0) {
		MIKU_DBG("ls_i2c_tx_byte SR_TIP can not ready!\n");
		ls_i2c_stop();
		return -1;
	}

	if (readb(i2c_base_addr + SR_REG) & SR_NOACK) {
		MIKU_DBG("device has no ack, Pls check the hardware!\n");
		ls_i2c_stop();
		return -1;
	}

	return 0;
}

static int ls_i2c_send_addr(unsigned char dev_addr,unsigned int data_addr)
{
	if (ls_i2c_tx_byte(dev_addr, CR_START | CR_WRITE) < 0)
		return 0;

	if (word_offset) {
	//some device need word size addr
		if (ls_i2c_tx_byte((data_addr >> 8) & 0xff, CR_WRITE) < 0)
			return 0;
	}
	if (ls_i2c_tx_byte(data_addr & 0xff, CR_WRITE) < 0)
		return 0;

	return 1;
}


 /*
 * the function write sequence data.
 * dev_addr : device id
 * data_addr : offset
 * buf : the write data buffer
 * count : size will be write
  */
int ls_i2c_write_seq(unsigned char dev_addr,unsigned int data_addr, unsigned char *buf, int count)
{
	int i;
	if (!ls_i2c_send_addr(dev_addr,data_addr))
		return 0;
	for (i = 0; i < count; i++)
		if (ls_i2c_tx_byte(buf[i] & 0xff, CR_WRITE) < 0)
			return 0;

	ls_i2c_stop();

	return i;
}

 /*
 * the function write one byte.
 * dev_addr : device id
 * data_addr : offset
 * buf : the write data
  */
int ls_i2c_write_byte(unsigned char dev_addr,unsigned int data_addr, unsigned char *buf)
{
	if (ls_i2c_write_seq(dev_addr, data_addr, buf, 1) == 1)
		return 0;
	return -1;
}
 /*
  * Sequential reads by a current address read.
 * dev_addr : device id
 * data_addr : offset
 * buf : the write data buffer
 * count : size will be write
  */
static int ls_i2c_read_seq_cur(unsigned char dev_addr,unsigned char *buf, int count)
{
	int i;
	dev_addr |= 0x1;

	if (ls_i2c_tx_byte(dev_addr, CR_START | CR_WRITE) < 0)
		return 0;

	for (i = 0; i < count; i++) {
		writeb(((i == count - 1) ? (CR_READ | CR_ACK) : CR_READ), i2c_base_addr + CR_REG);
		while (readb(i2c_base_addr + SR_REG) & SR_TIP) ;
		buf[i] = readb(i2c_base_addr + RXR_REG);
	}

	ls_i2c_stop();
	return i;
}

int ls_i2c_read_seq_rand(unsigned char dev_addr,unsigned int data_addr,
				unsigned char *buf, int count)
{
	if (!ls_i2c_send_addr(dev_addr,data_addr))
		return 0;

	return ls_i2c_read_seq_cur(dev_addr,buf, count);
}


void pmic_vctrl(u32 mv)
{
	u8 buf;
	i2c_base_addr = (void *)SMC_I2C0_BASE;
	ls_i2c_init();

	mv = (mv * 1000 - 210000) / 6587;

	buf = mv;
	MIKU_DBG("UPS9512s: Set VID: %d\n", buf);
	if(ls_i2c_write_byte(UPI9212S_ADDR0, 0x30, &buf))
		MIKU_DBG("write vid failed\n");

	MIKU_DBG("VID Set Done\n");
}