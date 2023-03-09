/************************************************************************
** File:
**   $Id: generic_imu_platform_cfg.h  $
**
** Purpose:
**  Define generic_imu Platform Configuration Parameters
**
** Notes:
**
*************************************************************************/
#ifndef _GENERIC_IMU_PLATFORM_CFG_H_
#define _GENERIC_IMU_PLATFORM_CFG_H_

/*
** Default GENERIC_IMU Configuration
*/
#ifndef GENERIC_IMU_CFG
    /* Notes: 
    **   NOS3 uart requires matching handle and bus number
    */
    #define GENERIC_IMU_CFG_STRING           "usart_29"
    #define GENERIC_IMU_CFG_HANDLE           29 
    #define GENERIC_IMU_CFG_BAUDRATE_HZ      115200
    #define GENERIC_IMU_CFG_MS_TIMEOUT       50            /* Max 255 */
    /* Note: Debug flag disabled (commented out) by default */
    //#define GENERIC_IMU_CFG_DEBUG
#endif

#endif /* _GENERIC_IMU_PLATFORM_CFG_H_ */
