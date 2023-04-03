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
#define GENERIC_IMU_DEVICE_RCV_HDR          0x00

#define GENERIC_IMU_DEVICE_NOOP_CMD         0x00
#define GENERIC_IMU_DEVICE_REQ_HK_CMD       0x01
#define GENERIC_IMU_DEVICE_REQ_X_DATA_CMD   0x02
#define GENERIC_IMU_DEVICE_REQ_Y_DATA_CMD   0x03
#define GENERIC_IMU_DEVICE_REQ_Z_DATA_CMD   0x04
//#define GENERIC_IMU_DEVICE_CFG_CMD          0x03

#define GENERIC_IMU_DEVICE_HDR_TRL_LEN      1
#define GENERIC_IMU_DEVICE_CMD_SIZE         2

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
** IMU Command Message
*/
typedef struct
{
    uint8_t     CmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint8_t     msg_type;
    uint8_t     cmd_id;
    uint8_t     src_mask;
    uint8_t     dest_mask;
    uint8_t     data_len;
    uint8_t     data[CAN_MAX_DLEN];
} OS_PACK GENERIC_IMU_Cmd_t;
#define GENERIC_IMU_CMD_LEN (sizeof(GENERIC_IMU_Cmd_t))


/*
** GENERIC_IMU device data definition for each individual axis
*/
typedef struct
{
    float       XLinearAcc;
    float       XAngularAcc;
} OS_PACK GENERIC_IMU_Device_X_Data_t;
#define GENERIC_IMU_DEVICE_X_DATA_LNGTH sizeof ( GENERIC_IMU_Device_X_Data_t )
#define GENERIC_IMU_DEVICE_X_DATA_SIZE GENERIC_IMU_DEVICE_X_DATA_LNGTH + GENERIC_IMU_DEVICE_HDR_TRL_LEN

typedef struct
{
    float       YLinearAcc;
    float       YAngularAcc;
} OS_PACK GENERIC_IMU_Device_Y_Data_t;
#define GENERIC_IMU_DEVICE_Y_DATA_LNGTH sizeof ( GENERIC_IMU_Device_Y_Data_t )
#define GENERIC_IMU_DEVICE_Y_DATA_SIZE GENERIC_IMU_DEVICE_Y_DATA_LNGTH + GENERIC_IMU_DEVICE_HDR_TRL_LEN

typedef struct
{
    float       ZLinearAcc;
    float       ZAngularAcc;
} OS_PACK GENERIC_IMU_Device_Z_Data_t;
#define GENERIC_IMU_DEVICE_Z_DATA_LNGTH sizeof ( GENERIC_IMU_Device_Z_Data_t )
#define GENERIC_IMU_DEVICE_Z_DATA_SIZE GENERIC_IMU_DEVICE_Z_DATA_LNGTH + GENERIC_IMU_DEVICE_HDR_TRL_LEN

/*
** GENERIC_IMU device data telemetry definition
*/
typedef struct
{
    uint32_t  DeviceCounter;
    
    GENERIC_IMU_Device_X_Data_t X_Data;
    GENERIC_IMU_Device_Y_Data_t Y_Data;
    GENERIC_IMU_Device_Z_Data_t Z_Data;
//    float     XLinearAcc;
//    float     XAngularAcc;
//    float     YLinearAcc;
//    float     YAngularAcc;
//    float     ZLinearAcc;
//    float     ZAngularAcc;

} OS_PACK GENERIC_IMU_Device_Data_tlm_t;
#define GENERIC_IMU_DEVICE_DATA_LNGTH sizeof ( GENERIC_IMU_Device_Data_tlm_t )
#define GENERIC_IMU_DEVICE_DATA_SIZE GENERIC_IMU_DEVICE_DATA_LNGTH + GENERIC_IMU_DEVICE_HDR_TRL_LEN


/*
** Prototypes
*/
void GENERIC_IMU_FramePrep(can_info_t *device, uint8_t* data, uint8_t data_len);
int32_t GENERIC_IMU_ReadData(can_info_t *canDevice, uint8_t data_length);
int32_t GENERIC_IMU_CommandDevice(can_info_t *canDevice, uint8_t cmd_code);
int32_t GENERIC_IMU_RequestHK(can_info_t *canDevice, GENERIC_IMU_Device_HK_tlm_t* data);
int32_t GENERIC_IMU_RequestData(can_info_t *canDevice, GENERIC_IMU_Device_Data_tlm_t* data);
int32_t GENERIC_IMU_RequestXData(can_info_t *canDevice, GENERIC_IMU_Device_X_Data_t* data);
int32_t GENERIC_IMU_RequestYData(can_info_t *canDevice, GENERIC_IMU_Device_Y_Data_t* data);
int32_t GENERIC_IMU_RequestZData(can_info_t *canDevice, GENERIC_IMU_Device_Z_Data_t* data);


#endif /* _GENERIC_IMU_DEVICE_H_ */
