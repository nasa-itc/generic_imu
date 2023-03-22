/*******************************************************************************
** File: generic_imu_device.h
**
** Purpose:
**   This is the header file for the GENERIC_IMU device.
**
*******************************************************************************/
#ifndef _GENERIC_IMU_DEVICE_H_
#define _GENERIC_IMU_DEVICE_H_

/*
** Required header files.
*/
#include "device_cfg.h"
#include "hwlib.h"
#include "generic_imu_platform_cfg.h"


/*
** Type definitions
** TODO: Make specific to your application
*/
#define GENERIC_IMU_DEVICE_HDR              0x80

#define GENERIC_IMU_DEVICE_NOOP_CMD         0x00
#define GENERIC_IMU_DEVICE_REQ_HK_CMD       0x01
#define GENERIC_IMU_DEVICE_REQ_DATA_CMD     0x02
#define GENERIC_IMU_DEVICE_CFG_CMD          0x03

#define GENERIC_IMU_DEVICE_HDR_TRL_LEN      2
#define GENERIC_IMU_DEVICE_CMD_SIZE         6

/*
** GENERIC_IMU device housekeeping telemetry definition
*/
typedef struct
{
    uint32_t  DeviceCounter;
    uint32_t  DeviceConfig;
    uint32_t  DeviceStatus;

} OS_PACK GENERIC_IMU_Device_HK_tlm_t;
#define GENERIC_IMU_DEVICE_HK_LNGTH sizeof ( GENERIC_IMU_Device_HK_tlm_t )
#define GENERIC_IMU_DEVICE_HK_SIZE GENERIC_IMU_DEVICE_HK_LNGTH + GENERIC_IMU_DEVICE_HDR_TRL_LEN


/*
** GENERIC_IMU device data telemetry definition
*/
typedef struct
{
    uint32_t  DeviceCounter;
    float     XLinearAcc;
    float     XAngularAcc;
    float     YLinearAcc;
    float     YAngularAcc;
    float     ZLinearAcc;
    float     ZAngularAcc;

} OS_PACK GENERIC_IMU_Device_Data_tlm_t;
#define GENERIC_IMU_DEVICE_DATA_LNGTH sizeof ( GENERIC_IMU_Device_Data_tlm_t )
#define GENERIC_IMU_DEVICE_DATA_SIZE GENERIC_IMU_DEVICE_DATA_LNGTH + GENERIC_IMU_DEVICE_HDR_TRL_LEN


/*
** Prototypes
*/
int32_t GENERIC_IMU_ReadData(can_info_t *canDevice, uint8_t data_length);
int32_t GENERIC_IMU_CommandDevice(can_info_t *canDevice, uint8_t cmd, uint32_t payload);
int32_t GENERIC_IMU_RequestHK(can_info_t *canDevice, GENERIC_IMU_Device_HK_tlm_t* data);
int32_t GENERIC_IMU_RequestData(can_info_t *canDevice, GENERIC_IMU_Device_Data_tlm_t* data);


#endif /* _GENERIC_IMU_DEVICE_H_ */
