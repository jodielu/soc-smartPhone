/***********************************************************************************************//**
 * \file   main.c
 * \brief  Silabs HTM IAS and Beaconing Demo Application
 *         This application is intended to be used with the iOS Silicon Labs
 *         app for demonstration purposes
 ***************************************************************************************************
 * <b> (C) Copyright 2015 Silicon Labs, http://www.silabs.com</b>
 ***************************************************************************************************
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 **************************************************************************************************/
#include "init_mcu.h"
#include "init_board.h"
#include "init_app.h"
#include "ble-configuration.h"
#include "board_features.h"
#include <time.h>
/* BG stack headers */
#include "bg_types.h"
#include "native_gecko.h"
#include "gatt_db.h"

/* application specific files */
#include "app.h"
#include "app_timer.h"
/* libraries containing default gecko configuration values */
#include "em_emu.h"
#include "em_cmu.h"

/* Device initialization header */
#include "hal-config.h"

#if defined(HAL_CONFIG)
#include "bsphalconfig.h"
#else
#include "bspconfig.h"
#endif

#include "bsp.h"
#include "bsp_trace.h"

#include "adc.h"
/***********************************************************************************************//**
 * @addtogroup Application
 * @{
 **************************************************************************************************/

/***********************************************************************************************//**
 * @addtogroup app
 * @{
 **************************************************************************************************/

#ifndef MAX_CONNECTIONS
#define MAX_CONNECTIONS 4
#endif
uint8_t bluetooth_stack_heap[DEFAULT_BLUETOOTH_HEAP(MAX_CONNECTIONS)];

// Gecko configuration parameters (see gecko_configuration.h)
static const gecko_configuration_t config = {
  .config_flags = 0,
  .sleep.flags = SLEEP_FLAGS_DEEP_SLEEP_ENABLE,
  .bluetooth.max_connections = MAX_CONNECTIONS,
  .bluetooth.heap = bluetooth_stack_heap,
  .bluetooth.heap_size = sizeof(bluetooth_stack_heap),
  .bluetooth.sleep_clock_accuracy = 100, // ppm
  .gattdb = &bg_gattdb_data,
  .ota.flags = 0,
  .ota.device_name_len = 3,
  .ota.device_name_ptr = "OTA",
#if (HAL_PA_ENABLE) && defined(FEATURE_PA_HIGH_POWER)
  .pa.config_enable = 1, // Enable high power PA
  .pa.input = GECKO_RADIO_PA_INPUT_VBAT, // Configure PA input to VBAT
#endif // (HAL_PA_ENABLE) && defined(FEATURE_PA_HIGH_POWER)
};



int main(void)
{
	//clock_t start;
	// AEM (Advanced Energy Monitor) setup for energy contribution breakdown
	//BSP_TraceSwoSetup();
	// Initialize device
	initMcu();
	// Initialize board
	initBoard();
	// Initialize application
	initApp();
	// Initialize LEDs
	BSP_LedsInit();
	//start = clock();
	//measTick();

	/* Setup MCU clock to 4 MHz */
	CMU_HFRCOFreqSet(cmuHFRCOFreq_4M0Hz);

	/* Enable atomic read-clear operation on reading IFC register */
	MSC->CTRL |= MSC_CTRL_IFCREADCLEAR;

	/* Initialize RTCC */
	rtccSetup();

	/* Initialize LDMA */
	ldmaSetup();

	/* Initialize ADC */
	adcSetup();

#ifndef FEATURE_LED_BUTTON_ON_SAME_PIN
  // Configure pin as input
  GPIO_PinModeSet(BSP_BUTTON0_PORT, BSP_BUTTON0_PIN, gpioModeInput, 1);
  // Configure pin as input
  GPIO_PinModeSet(BSP_BUTTON1_PORT, BSP_BUTTON1_PIN, gpioModeInput, 1);
#endif

  // Initialize stack
  gecko_init(&config);

  while (1) {
    struct gecko_cmd_packet* evt;
    // Check for stack event.
    evt = gecko_wait_event();
    // Run application and event handler.
    appHandleEvents(evt);
  }
  return 0;

}

/** @} (end addtogroup app) */
/** @} (end addtogroup Application) */
