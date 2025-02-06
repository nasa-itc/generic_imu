#ifndef _GENERIC_imu_CHECKOUT_DEVICE_CFG_H_
#define _GENERIC_imu_CHECKOUT_DEVICE_CFG_H_

/*
** GENERIC_imu Checkout Configuration
*/
#define GENERIC_imu_CFG
/* Note: NOS3 uart requires matching handle and bus number */
#define GENERIC_imu_CFG_STRING           "/dev/usart_16"
#define GENERIC_imu_CFG_HANDLE           16 
#define GENERIC_imu_CFG_BAUDRATE_HZ      115200
#define GENERIC_imu_CFG_MS_TIMEOUT       250
#define GENERIC_imu_CFG_DEBUG

#endif /* _GENERIC_imu_CHECKOUT_DEVICE_CFG_H_ */
