/* SPDX-License-Identifier: MIT OR Apache-2.0 */
/*
 * Copyright (C) 2019, 2020 Jiaxun Yang <jiaxun.yang@flygoat.com>
 */

#ifndef _MIKU_SENSORS_H__
#define _MIKU_SENSORS_H__

#include "miku.h"

#define MIKU_SENSORS_MAX 32

#define SENSOR_INFO_TYPE_TEMP 0x0
#define SENSOR_INFO_TYPE_VOLTAGE 0x1
#define SENSOR_INFO_TYPE_FLAGS 0xf
#define SENSOR_FLAG_TEMP (1 << 0)
#define SENSOR_FLAG_VOLTAGE (1 << 1)

struct sensor_info_args {
	union {
		u16 val;
		s16 temp; /* Signed 16bit, in Celsius */
	};
	u8 info_type : 4;
	u8 sensor_id : 4;
};

typedef rt_int16_t (* miku_sensor_get_temp_t)(void);
typedef rt_int16_t (* miku_sensor_get_volt_t)(void);

enum miku_sensor_ids {

#ifdef MIKU_SENSORS_HAVE_PACKAGE
	MIKU_SENSOR_ID_PACKAGE = 0x0,
#endif
	MIKU_SENSOR_ID_END,
};

extern rt_int16_t get_chiptemp(void);
extern rt_err_t miku_sensor_install_temp(rt_uint8_t id, miku_sensor_get_temp_t get_func);
extern rt_err_t miku_sensor_install_volt(rt_uint8_t id, miku_sensor_get_volt_t get_func);
extern rt_err_t miku_enable_sensors(void);
extern rt_err_t miku_sensors_init(void);
extern rt_int16_t miku_sensor_get_temp(rt_uint8_t id);


#endif