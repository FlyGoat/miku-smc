/* SPDX-License-Identifier: MIT OR Apache-2.0 */
/*
 * Copyright (C) 2019, 2020 Jiaxun Yang <jiaxun.yang@flygoat.com>
 */

#include "miku.h"
#include <math.h>

#define CHIPTEMP_REG HWREG32(SMC_CHIPTEMP_BASE)

struct miku_sensor
{
	rt_uint16_t flags;
	miku_sensor_get_temp_t get_temp;
	miku_sensor_get_volt_t get_volt;
};

static struct miku_sensor miku_sensors[MIKU_SENSOR_ID_END];

/* CSR 0x428 get temperature in celsius */
rt_int16_t get_chiptemp(void)
{
	rt_int32_t temp;

	temp = (CHIPTEMP_REG & 0xffff)*731;
	temp = temp/0x4000;
	temp = temp - 273;

	return (rt_int16_t)temp;
}

rt_err_t miku_sensor_install_temp(rt_uint8_t id, miku_sensor_get_temp_t get_func)
{
	if (id >= MIKU_SENSOR_ID_END)
		return RT_ERROR;
	miku_sensors[id].get_temp = get_func;
	miku_sensors[id].flags |= SENSOR_FLAG_TEMP;
	return RT_EOK;
}

rt_err_t miku_sensor_install_volt(rt_uint8_t id, miku_sensor_get_volt_t get_func)
{
	if (id >= MIKU_SENSOR_ID_END)
		return RT_ERROR;
	miku_sensors[id].get_volt = get_func;
	miku_sensors[id].flags |= SENSOR_FLAG_VOLTAGE;
	return RT_EOK;
}

rt_int16_t miku_sensor_get_temp(rt_uint8_t id)
{
	if(!(miku_sensors[id].flags & SENSOR_FLAG_TEMP))
		return -1;

	return miku_sensors[id].get_temp();
}

static rt_uint8_t miku_cmd_get_sensor_num(struct smc_message *msg)
{
	msg->arg = MIKU_SENSOR_ID_END;
	return MIKU_EOK;
}


static rt_uint8_t miku_cmd_get_sensor_status(struct smc_message *msg)
{
	rt_uint32_t argp = msg->arg;
	struct sensor_info_args *arg = (struct sensor_info_args*)&argp;

	MIKU_DBG("Sensors: get sensor status id: %x, type: %x", arg->sensor_id, arg->info_type);
	if (arg->sensor_id >= MIKU_SENSOR_ID_END)
		return MIKU_ECMDFAIL;
	
	if (arg->info_type == SENSOR_INFO_TYPE_FLAGS) {
		arg->val = miku_sensors[arg->sensor_id].flags;
	} else if (arg->info_type == SENSOR_INFO_TYPE_TEMP) {
		if(!(miku_sensors[arg->sensor_id].flags & SENSOR_FLAG_TEMP))
			return MIKU_ECMDFAIL;
		arg->temp = miku_sensors[arg->sensor_id].get_temp();
	} else if (arg->info_type == SENSOR_INFO_TYPE_VOLTAGE) {
		if(!(miku_sensors[arg->sensor_id].flags & SENSOR_FLAG_VOLTAGE))
			return MIKU_ECMDFAIL;
		arg->val = miku_sensors[arg->sensor_id].get_volt();
	} else {
		return MIKU_ECMDFAIL;
	}

	msg->value = argp;
	return MIKU_EOK;
}

rt_err_t miku_enable_sensors(void)
{
	miku_cmd_install(CMD_GET_SENSOR_NUM, miku_cmd_get_sensor_num);
	miku_cmd_install(CMD_GET_SENSOR_STATUS, miku_cmd_get_sensor_status);
	return RT_EOK;
}

static rt_err_t miku_sensors_init_cputemp(void)
{
	miku_sensor_install_temp(MIKU_SENSOR_ID_PACKAGE, get_chiptemp);

	return RT_EOK;
}

rt_err_t miku_sensors_init(void)
{
	miku_sensors_init_cputemp();
}