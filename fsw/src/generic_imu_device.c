/*******************************************************************************
** File: generic_imu_device.c
**
** Purpose:
**   This file contains the source code for the GENERIC_IMU device.
**
*******************************************************************************/

/*
** Include Files
*/
#include "generic_imu_device.h"

// Both of the below constants are defined to allow the greatest possible precision
// without overflow on a range of -10<x<10 for x=linear acceleration (in g) and
// -400<x<400 for x=angular rotation rate (in deg/s).
#define LIN_CONV_CONST 214748364 //multiply by 10 to get the added constant
#define ANG_CONV_CONST 5368709 //multiply by 400 to get the added constant

/*
** Helper Function
*/
void GENERIC_IMU_FramePrep(can_info_t *device, uint8_t* data, uint8_t data_len)
{
    /* TX Frame */
    //device->tx_frame.can_id = ((uint32_t)(cmd->msg_type & 0x1F) << 24) + 
    //                          ((uint32_t)(cmd->cmd_id) << 16) + 
    //                          ((uint32_t)(cmd->src_mask) << 8) + 
    //                          ((uint32_t)(cmd->dest_mask));    
    device->tx_frame.can_dlc = data_len;
    CFE_PSP_MemCpy((void*)device->tx_frame.data, data, CAN_MAX_DLEN);
    
    /* RX Frame */
    device->rx_frame.can_id = 0;
    device->rx_frame.can_dlc = 0;
    CFE_PSP_MemSet((void*)device->rx_frame.data, 0x00, CAN_MAX_DLEN);
}

/* 
** Generic read data from device
*/
int32_t GENERIC_IMU_ReadData(can_info_t *canDevice, uint8_t data_length)
{
    int32_t status = OS_SUCCESS;

    /* Wait until all data received or timeout occurs */
    status = can_master_transaction(canDevice);
    if (status != CAN_SUCCESS)
    {
        #ifdef GENERIC_IMU_CFG_DEBUG
            OS_printf("GENERIC_IMU_ReadData: GENERIC_IMU_ReadData can_master_transaction failed with %d error! \n", status);
        #endif
        status = CAN_ERROR;
    }
    return status;
}


/* 
** Generic command to device
** Note that confirming the echoed response is specific to this implementation
*/
int32_t GENERIC_IMU_CommandDevice(can_info_t *canDevice, uint8_t cmd_code)
{
    int32_t status = OS_SUCCESS;
    int32_t bytes = 0;
    uint8_t write_data[GENERIC_IMU_DEVICE_CMD_SIZE] = {0};
    uint8_t read_data[GENERIC_IMU_DEVICE_DATA_SIZE] = {0};

    /* Prepare command */
    write_data[0] = GENERIC_IMU_DEVICE_HDR;
    write_data[1] = cmd_code;

    GENERIC_IMU_FramePrep(canDevice, write_data, GENERIC_IMU_DEVICE_CMD_SIZE);
    status = GENERIC_IMU_ReadData(canDevice, GENERIC_IMU_DEVICE_CMD_SIZE);
    if (status != OS_SUCCESS)
    {
        #ifdef GENERIC_IMU_CFG_DEBUG
        OS_printf("GENERIC_IMU_CommandDevice - GENERIC_IMU_ReadData returned %d \n", status);
        #endif
    }
    return status;
}


/*
** Request housekeeping command
*/
int32_t GENERIC_IMU_RequestHK(can_info_t *canDevice, GENERIC_IMU_Device_HK_tlm_t* data)
{
    int32_t status = OS_SUCCESS;
    uint8_t read_data[GENERIC_IMU_DEVICE_HK_SIZE] = {0};

    status = GENERIC_IMU_CommandDevice(canDevice, 1);
    if (status == OS_SUCCESS)
    {
        #ifdef GENERIC_IMU_CFG_DEBUG
            OS_printf("  GENERIC_IMU_RequestHK[%d] = ", canDevice->rx_frame.can_dlc);
            for (uint32_t i = 0; i < canDevice->rx_frame.can_dlc; i++)
            {
                OS_printf("%02x", canDevice->rx_frame.data[i]);
            }
            OS_printf("\n");
        #endif

        /* Verify return frame length
        if (canDevice->rx_frame.can_dlc != GENERIC_IMU_DEVICE_HK_SIZE)
        {
            #ifdef GENERIC_IMU_CFG_DEBUG
                OS_printf("  GENERIC_IMU_RequestHK: Bytes read != to requested! \n");
            #endif
            status = OS_ERROR;
        }
        */

        /* Verify data header and trailer */
        if ((read_data[0]  == GENERIC_IMU_DEVICE_HDR))
        {
            data->DeviceCounter  = read_data[2] << 24;
            data->DeviceCounter |= read_data[3] << 16;
            data->DeviceCounter |= read_data[4] << 8;
            data->DeviceCounter |= read_data[5];

            data->DeviceConfig  = read_data[6] << 24;
            data->DeviceConfig |= read_data[7] << 16;
            data->DeviceConfig |= read_data[8] << 8;
            data->DeviceConfig |= read_data[9];

            data->DeviceStatus  = read_data[10] << 24;
            data->DeviceStatus |= read_data[11] << 16;
            data->DeviceStatus |= read_data[12] << 8;
            data->DeviceStatus |= read_data[13];

            #ifdef GENERIC_IMU_CFG_DEBUG
                OS_printf("  Header  = 0x%02x      \n", read_data[0]);
                OS_printf("  Counter = 0x%08x      \n", data->DeviceCounter);
                OS_printf("  Config  = 0x%08x      \n", data->DeviceConfig);
                OS_printf("  Status  = 0x%08x      \n", data->DeviceStatus);
                OS_printf("  Trailer = 0x%02x%02x  \n", read_data[14], read_data[15]);
            #endif
        }
        else
        {
            #ifdef GENERIC_IMU_CFG_DEBUG
                OS_printf("  GENERIC_IMU_RequestHK: GENERIC_IMU_CommandDevice reported error %d \n", status);
            #endif 
            status = OS_ERROR;
        }
    }
    return status;
}

/*
** Command to request X data
*/
int32_t GENERIC_IMU_RequestXData(can_info_t *canDevice, GENERIC_IMU_Device_X_Data_t* data)
{
    int32_t status = OS_SUCCESS;
    uint8_t read_data[GENERIC_IMU_DEVICE_DATA_SIZE] = {0};

    status = GENERIC_IMU_CommandDevice(canDevice, 2);
    if (status == OS_SUCCESS)
    {
        #ifdef GENERIC_IMU_CFG_DEBUG
            OS_printf("  GENERIC_IMU_RequestXData[%d] = ", canDevice->rx_frame.can_dlc);
            for (uint32_t i = 0; i < canDevice->rx_frame.can_dlc; i++)
            {
                OS_printf("%02x", canDevice->rx_frame.data[i]);
            }
            OS_printf("\n");
        #endif

        /* Verify data header and trailer */
        if ((read_data[0]  == GENERIC_IMU_DEVICE_HDR))
        {
//            data->DeviceCounter  = read_data[2] << 24;
//            data->DeviceCounter |= read_data[3] << 16;
//            data->DeviceCounter |= read_data[4] << 8;
//            data->DeviceCounter |= read_data[5];

            uint32_t data_preadjustment[2];
            for (int i = 1; i <= 2; i++) {
                data_preadjustment[i-1]  = read_data[2+4*i] << 24;
                data_preadjustment[i-1] |= read_data[3+4*i] << 16;
                data_preadjustment[i-1] |= read_data[4+4*i] << 8;
                data_preadjustment[i-1] |= read_data[5+4*i];

            }

            data->XLinearAcc  = (float) (data_preadjustment[0] - LIN_CONV_CONST*10)/LIN_CONV_CONST;
            data->XAngularAcc  = (float) (data_preadjustment[1] - ANG_CONV_CONST*400)/ANG_CONV_CONST;

            #ifdef GENERIC_IMU_CFG_DEBUG
                OS_printf("  Header  = 0x%02x%02x  \n", read_data[0], read_data[1]);
//                OS_printf("  Counter = 0x%08x      \n", data->DeviceCounter);
                OS_printf("  Linear X  = 0x%04x, %d  \n", data->XLinearAcc, data->XLinearAcc);
                OS_printf("  Angular X = 0x%04x, %d  \n", data->XAngularAcc, data->XAngularAcc);
                OS_printf("  Trailer = 0x%02x%02x  \n", read_data[10], read_data[11]);
            #endif
        }
    } 
    else
    {
        #ifdef GENERIC_IMU_CFG_DEBUG
            OS_printf("  GENERIC_IMU_RequestXData: Invalid data read! \n");
        #endif 
        status = OS_ERROR;
    } /* GENERIC_IMU_ReadXData */
    return status;
}

/*
** Command to request Y data
*/
int32_t GENERIC_IMU_RequestYData(can_info_t *canDevice, GENERIC_IMU_Device_Y_Data_t* data)
{
    int32_t status = OS_SUCCESS;
    uint8_t read_data[GENERIC_IMU_DEVICE_DATA_SIZE] = {0};

    status = GENERIC_IMU_CommandDevice(canDevice, 3);
    if (status == OS_SUCCESS)
    {
        #ifdef GENERIC_IMU_CFG_DEBUG
            OS_printf("  GENERIC_IMU_RequestYData[%d] = ", canDevice->rx_frame.can_dlc);
            for (uint32_t i = 0; i < canDevice->rx_frame.can_dlc; i++)
            {
                OS_printf("%02x", canDevice->rx_frame.data[i]);
            }
            OS_printf("\n");
        #endif

        /* Verify data header and trailer */
        if ((read_data[0]  == GENERIC_IMU_DEVICE_HDR))
        {
//            data->DeviceCounter  = read_data[2] << 24;
//            data->DeviceCounter |= read_data[3] << 16;
//            data->DeviceCounter |= read_data[4] << 8;
//            data->DeviceCounter |= read_data[5];

            uint32_t data_preadjustment[2];
            for (int i = 1; i <= 2; i++) {
                data_preadjustment[i-1]  = read_data[2+4*i] << 24;
                data_preadjustment[i-1] |= read_data[3+4*i] << 16;
                data_preadjustment[i-1] |= read_data[4+4*i] << 8;
                data_preadjustment[i-1] |= read_data[5+4*i];

            }

            data->YLinearAcc  = (float) (data_preadjustment[0] - LIN_CONV_CONST*10)/LIN_CONV_CONST;
            data->YAngularAcc  = (float) (data_preadjustment[1] - ANG_CONV_CONST*400)/ANG_CONV_CONST;

            #ifdef GENERIC_IMU_CFG_DEBUG
                OS_printf("  Header  = 0x%02x%02x  \n", read_data[0], read_data[1]);
//                OS_printf("  Counter = 0x%08x      \n", data->DeviceCounter);
                OS_printf("  Linear Y  = 0x%04x, %d  \n", data->YLinearAcc, data->YLinearAcc);
                OS_printf("  Angular Y = 0x%04x, %d  \n", data->YAngularAcc, data->YAngularAcc);
                OS_printf("  Trailer = 0x%02x%02x  \n", read_data[10], read_data[11]);
            #endif
        }
    } 
    else
    {
        #ifdef GENERIC_IMU_CFG_DEBUG
            OS_printf("  GENERIC_IMU_RequestYData: Invalid data read! \n");
        #endif 
        status = OS_ERROR;
    } /* GENERIC_IMU_ReadYData */
    return status;
}

/*
** Command to request Z data
*/
int32_t GENERIC_IMU_RequestZData(can_info_t *canDevice, GENERIC_IMU_Device_Z_Data_t* data)
{
    int32_t status = OS_SUCCESS;
    uint8_t read_data[GENERIC_IMU_DEVICE_DATA_SIZE] = {0};

    status = GENERIC_IMU_CommandDevice(canDevice, 4);
    if (status == OS_SUCCESS)
    {
        #ifdef GENERIC_IMU_CFG_DEBUG
            OS_printf("  GENERIC_IMU_RequestZData[%d] = ", canDevice->rx_frame.can_dlc);
            for (uint32_t i = 0; i < canDevice->rx_frame.can_dlc; i++)
            {
                OS_printf("%02x", canDevice->rx_frame.data[i]);
            }
            OS_printf("\n");
        #endif

        /* Verify data header and trailer */
        if ((read_data[0]  == GENERIC_IMU_DEVICE_HDR))
        {
//            data->DeviceCounter  = read_data[2] << 24;
//            data->DeviceCounter |= read_data[3] << 16;
//            data->DeviceCounter |= read_data[4] << 8;
//            data->DeviceCounter |= read_data[5];

            uint32_t data_preadjustment[2];
            for (int i = 1; i <= 2; i++) {
                data_preadjustment[i-1]  = read_data[2+4*i] << 24;
                data_preadjustment[i-1] |= read_data[3+4*i] << 16;
                data_preadjustment[i-1] |= read_data[4+4*i] << 8;
                data_preadjustment[i-1] |= read_data[5+4*i];

            }

            data->ZLinearAcc  = (float) (data_preadjustment[0] - LIN_CONV_CONST*10)/LIN_CONV_CONST;
            data->ZAngularAcc  = (float) (data_preadjustment[1] - ANG_CONV_CONST*400)/ANG_CONV_CONST;

            #ifdef GENERIC_IMU_CFG_DEBUG
                OS_printf("  Header  = 0x%02x%02x  \n", read_data[0], read_data[1]);
//                OS_printf("  Counter = 0x%08x      \n", data->DeviceCounter);
                OS_printf("  Linear Z  = 0x%04x, %d  \n", data->ZLinearAcc, data->ZLinearAcc);
                OS_printf("  Angular Z = 0x%04x, %d  \n", data->ZAngularAcc, data->ZAngularAcc);
                OS_printf("  Trailer = 0x%02x%02x  \n", read_data[10], read_data[11]);
            #endif
        }
    } 
    else
    {
        #ifdef GENERIC_IMU_CFG_DEBUG
            OS_printf("  GENERIC_IMU_RequestZData: Invalid data read! \n");
        #endif 
        status = OS_ERROR;
    } /* GENERIC_IMU_ReadZData */
    return status;
}



/*
** Request data command
*/
/*
int32_t GENERIC_IMU_RequestData(can_info_t *canDevice, GENERIC_IMU_Device_Data_tlm_t* data)
{
    int32_t status = OS_SUCCESS;
    uint8_t read_data[GENERIC_IMU_DEVICE_DATA_SIZE] = {0};

    status = GENERIC_IMU_CommandDevice(canDevice, 2);
    if (status == OS_SUCCESS)
    {
        #ifdef GENERIC_IMU_CFG_DEBUG
            OS_printf("  GENERIC_IMU_RequestData[%d] = ", canDevice->rx_frame.can_dlc);
            for (uint32_t i = 0; i < canDevice->rx_frame.can_dlc; i++)
            {
                OS_printf("%02x", canDevice->rx_frame.data[i]);
            }
            OS_printf("\n");
        #endif
*/
        /* Verify return frame length
        if (canDevice->rx_frame.can_dlc != GENERIC_IMU_DEVICE_DATA_SIZE)
        {
            #ifdef GENERIC_IMU_CFG_DEBUG
                OS_printf("  GENERIC_IMU_RequestData: Bytes read != to requested! \n");
            #endif
            status = OS_ERROR;
        }
        */

        /* Verify data header and trailer */
/*
        if ((read_data[0]  == GENERIC_IMU_DEVICE_HDR))
        {
            data->DeviceCounter  = read_data[2] << 24;
            data->DeviceCounter |= read_data[3] << 16;
            data->DeviceCounter |= read_data[4] << 8;
            data->DeviceCounter |= read_data[5];

            uint32_t data_preadjustment[6];
            for (int i = 1; i <= 6; i++) {
                data_preadjustment[i-1]  = read_data[2+4*i] << 24;
                data_preadjustment[i-1] |= read_data[3+4*i] << 16;
                data_preadjustment[i-1] |= read_data[4+4*i] << 8;
                data_preadjustment[i-1] |= read_data[5+4*i];

            }

            data->XLinearAcc  = (float) (data_preadjustment[0] - LIN_CONV_CONST*10)/LIN_CONV_CONST;
            data->XAngularAcc  = (float) (data_preadjustment[1] - ANG_CONV_CONST*400)/ANG_CONV_CONST;
            data->YLinearAcc  = (float) (data_preadjustment[2] - LIN_CONV_CONST*10)/LIN_CONV_CONST;
            data->YAngularAcc  = (float) (data_preadjustment[3] - ANG_CONV_CONST*400)/ANG_CONV_CONST;
            data->ZLinearAcc  = (float) (data_preadjustment[4] - LIN_CONV_CONST*10)/LIN_CONV_CONST;
            data->ZAngularAcc  = (float) (data_preadjustment[5] - ANG_CONV_CONST*400)/ANG_CONV_CONST;

//                data->XLinearAcc  = read_data[6] << 24;
//                data->XLinearAcc |= read_data[7] << 16;
//                data->XLinearAcc |= read_data[8] << 8;
//                data->XLinearAcc |= read_data[9];
//
//                data->XAngularAcc  = read_data[10] << 24;
//
//                data->YLinearAcc   = read_data[14] << 24;
//                data->YAngularAcc  = read_data[18] << 24;
//               
//                data->ZLinearAcc   = read_data[22] << 24;
//                data->ZAngularAcc  = read_data[26] << 24;

//                data->DeviceDataZ  = read_data[10] << 8;
//                data->DeviceDataZ |= read_data[11];

            #ifdef GENERIC_IMU_CFG_DEBUG
                OS_printf("  Header  = 0x%02x%02x  \n", read_data[0], read_data[1]);
                OS_printf("  Counter = 0x%08x      \n", data->DeviceCounter);
                OS_printf("  Linear X  = 0x%04x, %d  \n", data->XLinearAcc, data->XLinearAcc);
                OS_printf("  Angular X = 0x%04x, %d  \n", data->XAngularAcc, data->XAngularAcc);
                OS_printf("  Linear Y  = 0x%04x, %d  \n", data->YLinearAcc, data->YLinearAcc);
                OS_printf("  Angular Y = 0x%04x, %d  \n", data->YAngularAcc, data->YAngularAcc);
                OS_printf("  Linear Z  = 0x%04x, %d  \n", data->ZLinearAcc, data->ZLinearAcc);
                OS_printf("  Angular Z = 0x%04x, %d  \n", data->ZAngularAcc, data->ZAngularAcc);
                OS_printf("  Trailer = 0x%02x%02x  \n", read_data[10], read_data[11]);
            #endif
        }
    } 
    else
    {
        #ifdef GENERIC_IMU_CFG_DEBUG
            OS_printf("  GENERIC_IMU_RequestData: Invalid data read! \n");
        #endif 
        status = OS_ERROR;
    }*/ /* GENERIC_IMU_ReadData */
/*    return status;
}
*/
