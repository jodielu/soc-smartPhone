/***********************************************************************************************//**
 * \file   app_hw.h
 * \brief  Hardware specific application header file
 ***************************************************************************************************
 * <b> (C) Copyright 2015 Silicon Labs, http://www.silabs.com</b>
 ***************************************************************************************************
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 **************************************************************************************************/

#ifndef APP_HW_H
#define APP_HW_H

#ifdef __cplusplus
extern "C" {
#endif

#include "em_device.h"
#include <stdbool.h>

/***********************************************************************************************//**
 * \defgroup app_hw Application Hardware Specific
 * \brief Hardware specific application file.
 **************************************************************************************************/

/***********************************************************************************************//**
 * @addtogroup Application
 * @{
 **************************************************************************************************/

/***********************************************************************************************//**
 * @addtogroup app
 * @{
 **************************************************************************************************/

/***************************************************************************************************
 * Data Types
 **************************************************************************************************/

/***************************************************************************************************
 * Function Declarations
 **************************************************************************************************/

/***********************************************************************************************//**
 *  \brief  Initialize buttons and Temperature sensor.
 **************************************************************************************************/
void appHwInit(void);

/***********************************************************************************************//**
 *  \brief  Perform a temperature & relative humidity measurement.  Return the measurement data.
 *  \param[out]  tempData  Result of temperature conversion.
 *  			 rhData    Result of relative humidity.
 *  \return  0 if Read successful, otherwise -1
 **************************************************************************************************/
int32_t appHwReadTm(int32_t* tempData, uint32_t* rhData);

/***********************************************************************************************//**
 *  \brief  Initialise temperature measurement.
 *  \return  true if a Si7013 is detected, false otherwise
 **************************************************************************************************/
bool appHwInitTempSens(void);
/*******************************************************************************
 *****************************   LDC1612   **********************************
 ******************************************************************************/
int32_t appHwReadFreq(uint32_t* freqData0, uint32_t* freqData1);
bool appHwInitFreqSens(uint16_t* deviceId);
int32_t appHwReadFlash(uint32_t* MX25ID);
/** @} (end addtogroup app_hw) */
/** @} (end addtogroup Application) */

#ifdef __cplusplus
};
#endif

#endif /* APP_HW_H */
