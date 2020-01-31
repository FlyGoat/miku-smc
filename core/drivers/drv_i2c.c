/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-12-10     Jiaxun Yang        first implementation.
 */
#include <rtthread.h>
#include <mips.h>
#include <rtdevice.h>
#include <rthw.h>
#include "ls3-smc.h"

#define DBG_TAG               "drv.I2C"
#ifdef RT_I2C_DEBUG
#define DBG_LVL               DBG_LOG
#else
#define DBG_LVL               DBG_INFO
#endif
#include <rtdbg.h>

#define PRER_LO(x)	REG8(x + 0x0)
#define PRER_HI(x)	REG8(x + 0x1)

#define CTR(x)	HWREG8(x + 0x2)
#define CTR_EN	(1 << 7)
#define CTR_IEN	(1 << 6) /* Enable Interrupt */
#define CTR_MST_EN	(1 << 5) /* Enable Master Mode */

#define	DAT(x)	HWREG8(x + 0x3) /* RXR/TXR */
#define DAT_DRW	(1 << 0)

#define CR(x)	HWREG8(x + 0x4) /* Command Registers */
#define CR_IAK	(1 << 0)
#define CR_NOACK	(1 << 3) /* Set this bit if you don't want it Ack after this command */
#define CR_WR	(1 << 4)
#define CR_RD	(1 << 5)
#define CR_STO	(1 << 6) /* STOP */
#define CR_STA	(1 << 7) /* START */

#define SR(x)	HWREG8(x + 0x4) /* Status Register */
#define SR_IF	(1 << 0)
#define SR_TIP	(1 << 1)
#define	SR_AL	(1 << 5)
#define	SR_BUSY	(1 << 6)
#define SR_RXACK	(1 << 7)

#define SLV_CTRL(x) HWREG8(x + 0x7)

#define XFER_MAX_RETRY	5

struct smc_i2c_bus
{
    struct rt_i2c_bus_device parent;
    void *reg;
};

static rt_err_t i2c_wait_sr_clear(struct smc_i2c_bus *dev, rt_uint8_t flags, rt_uint32_t retries)
{
    rt_uint32_t i;
    for (i = 0; i < retries; i++)
    {
        if(!(SR(dev->reg) & flags)) {
            rt_kprintf("i2c: succeed to wait sr: %x\n", SR(dev->reg));
            return RT_EOK;
        }
    }
    rt_kprintf("i2c: failed to wait sr: %x\n", SR(dev->reg));
    return RT_ERROR;
}

static void smc_i2c_stop(struct smc_i2c_bus *dev)
{
again:
    CR(dev->reg) = CR_STO;
        while (CR(dev->reg) & SR_BUSY)
                goto again;
}

static int smc_i2c_start(struct smc_i2c_bus *dev,
        unsigned char dev_addr, rt_bool_t read, rt_bool_t ignore_nack)
{
    int retry = 5;
    unsigned char addr = (dev_addr & 0x7f) << 1;
    if (read)
        addr |= DAT_DRW;

start:
    rt_kprintf("%s <line%d>: i2c device address: 0x%x\n",
            __func__, __LINE__, addr);
    DAT(dev->reg) = addr;
    CR(dev->reg) = (CR_STA | CR_WR);

    i2c_wait_sr_clear(dev, SR_TIP, 1000);

    if (i2c_wait_sr_clear(dev, SR_RXACK, 1000) != RT_EOK && !ignore_nack) {
        rt_kprintf("I2C Strat Failed Retry\n");
        smc_i2c_stop(dev);
        while (retry--)
            goto start;
        return 0;
    }

    rt_kprintf("start send done\n");
    return 1;
}

static int smc_i2c_read(struct smc_i2c_bus *dev,
        unsigned char *buf, int count)
{
    int i;

    for (i = 0; i < count; i++) {
        CR(dev->reg) = ((i == count - 1) ? (CR_RD | CR_NOACK) : CR_RD);
        while (SR(dev->reg) & SR_TIP);
        buf[i] = DAT(dev->reg);
        rt_kprintf("%s <line%d>: read buf[%d] <= %02x\n",
                __func__, __LINE__, i, buf[i]);
        }

        return i;
}

static int smc_i2c_write(struct smc_i2c_bus *dev,
        unsigned char *buf, int count, rt_bool_t ignore_nack)
{
        int i;

        for (i = 0; i < count; i++) {

        rt_kprintf("%s <line%d>: write buf[%d] => %02x\n",
                __func__, __LINE__, i, buf[i]);

        DAT(dev->reg) = buf[i];
        CR(dev->reg) = CR_WR;

        i2c_wait_sr_clear(dev, SR_TIP, 1000);

        if (i2c_wait_sr_clear(dev, SR_RXACK, 1000) != RT_EOK && !ignore_nack) {
                rt_kprintf("%s <line%d>: device no ack\n",
                            __func__, __LINE__);
            return i;
            }
        }

        return i;
}

static rt_size_t smc_i2c_xfer(struct rt_i2c_bus_device *bus,
                              struct rt_i2c_msg         msgs[],
                              rt_uint32_t               num)
{
    struct rt_i2c_msg *msg;
    struct smc_i2c_bus *dev = (struct smc_i2c_bus *)bus->priv;
    rt_int32_t i, ret;
    rt_bool_t ignore_nack;

    LOG_D("send start condition");
    for (i = 0; i < num; i++)
    {
        msg = &msgs[i];
        ignore_nack = msg->flags & RT_I2C_IGNORE_NACK;
        if (!(msg->flags & RT_I2C_NO_START))
        {
            smc_i2c_start(dev, msg->addr, msg->flags & RT_I2C_RD, ignore_nack);
        }
        if (msg->flags & RT_I2C_RD)
        {
            rt_kprintf("start reading\n");
            ret = smc_i2c_read(dev, (unsigned char *)msg->buf, msg->len);
            if (ret >= 1)
                LOG_D("read %d byte%s", ret, ret == 1 ? "" : "s");
            if (ret < msg->len)
            {
                if (ret >= 0)
                    ret = -RT_EIO;
                goto out;
            }
        }
        else
        {
            rt_kprintf("start writing\n");
            ret = smc_i2c_write(dev, (unsigned char *)msg->buf, msg->len, ignore_nack);
            if (ret >= 1)
                LOG_D("write %d byte%s", ret, ret == 1 ? "" : "s");
            if (ret < msg->len)
            {
                if (ret >= 0)
                    ret = -RT_ERROR;
                goto out;
            }
        }
    }
    ret = i;

out:
    LOG_D("send stop condition");
    smc_i2c_stop(dev);

    return ret;
}

static void smc_i2c_init(struct smc_i2c_bus *dev)
{
    rt_kprintf("SMC I2C INIT\n");

    CTR(dev->reg) = CTR_MST_EN;
    /* Copy Bonito_100M From PMON */
    PRER_LO(dev->reg) = 0x71;
    PRER_HI(dev->reg) = 0x1;
    CTR(dev->reg) = CTR_EN | CTR_MST_EN;
}

static const struct rt_i2c_bus_device_ops i2c_ops =
{
    smc_i2c_xfer,
    RT_NULL,
    RT_NULL
};

static struct smc_i2c_bus ls3_i2c0;

int rt_hw_i2c_init(void)
{
    struct smc_i2c_bus *dev;
    struct rt_i2c_bus_device *i2c_bus;

    dev = &ls3_i2c0;
    rt_memset((void *)dev, 0, sizeof(struct smc_i2c_bus));
    dev->reg = (void *) SMC_I2C0_BASE;
    i2c_bus = &dev->parent;
    i2c_bus->ops = &i2c_ops;
    i2c_bus->priv = (void *)dev;
    smc_i2c_init(dev);
    rt_i2c_bus_device_register(i2c_bus, "i2c0");
    return 0;
}

INIT_DEVICE_EXPORT(rt_hw_i2c_init);