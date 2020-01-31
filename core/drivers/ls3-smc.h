#ifndef _LS3_SMC_H__
#define _LS3_SMC_H__

#include <mips.h>

#define SMC_I2C0_BASE KSEG1ADDR(0x1fe00120)
#define SMC_I2C1_BASE KSEG1ADDR(0x1fe00130)
#define LS3_CORE_SCALE_REG KSEG1ADDR(0x1fe001d0)
#define UART0_BASE KSEG1ADDR(0x1fe001e0)
#define UART1_BASE KSEG1ADDR(0x1fe001e8)
#define SMC_CHIPTEMP_BASE KSEG1ADDR(0x1fe0019c)
#define SMC_MAILBOX_BASE KSEG1ADDR(0x1fe0051c)

#define MAIN_PLL_BASE KSEG1ADDR(0x1fe001b0)

#define LS7A_MISC_BASE			KSEG1ADDR(0x10080000)
#define LS7A_PWM0_REG_BASE		(LS7A_MISC_BASE + 0x20000)
#define LS7A_PWM0_LOW                   (LS7A_PWM0_REG_BASE + 0x4)
#define LS7A_PWM0_FULL                  (LS7A_PWM0_REG_BASE + 0x8)
#define LS7A_PWM0_CTRL                  (LS7A_PWM0_REG_BASE + 0xc)

#define LS7A_PWM1_REG_BASE		(LS7A_MISC_BASE + 0x20100)
#define LS7A_PWM1_LOW                   (LS7A_PWM1_REG_BASE + 0x4)
#define LS7A_PWM1_FULL                  (LS7A_PWM1_REG_BASE + 0x8)
#define LS7A_PWM1_CTRL                  (LS7A_PWM1_REG_BASE + 0xc)

#define LS7A_PWM2_REG_BASE		(LS7A_MISC_BASE + 0x20200)
#define LS7A_PWM2_LOW                   (LS7A_PWM2_REG_BASE + 0x4)
#define LS7A_PWM2_FULL                  (LS7A_PWM2_REG_BASE + 0x8)
#define LS7A_PWM2_CTRL                  (LS7A_PWM2_REG_BASE + 0xc)

#define LS7A_PWM3_REG_BASE		(LS7A_MISC_BASE + 0x20300)
#define LS7A_PWM3_LOW                   (LS7A_PWM3_REG_BASE + 0x4)
#define LS7A_PWM3_FULL                  (LS7A_PWM3_REG_BASE + 0x8)
#define LS7A_PWM3_CTRL                  (LS7A_PWM3_REG_BASE + 0xc)

#define LS7A_RTC_REG_BASE		(LS7A_MISC_BASE + 0x50100)
#define LS7A_RTC_RTCREAD		(LS7A_RTC_REG_BASE + 0x68)

#ifndef __ASSEMBLY__
#include <rthw.h>
void rt_hw_timer_handler(void);
void rt_hw_uart_init(void);
#endif

#endif