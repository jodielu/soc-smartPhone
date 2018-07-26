/*
 * adc.h
 *
 *  Created on: Jul 17, 2018
 *      Author: memsj
 */

#ifndef ADC_H_
#define ADC_H_
#include <stdio.h>
#include "em_chip.h"
#include "em_cmu.h"
#include "em_device.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_prs.h"
#include "em_rtcc.h"
#include "em_adc.h"
#include "em_ldma.h"

/* Defined for ADC */
#define ADC_CLOCK               1000000                 /* ADC conversion clock */
#define ADC_ASYNC_CLOCK         cmuAUXHFRCOFreq_4M0Hz   /* Clock for ASYNC mode */
#define ADC_INPUT0              adcPosSelAPORT3XCH8     /* PA0 */
#define ADC_INPUT1              adcPosSelAPORT3YCH9     /* PA1 */
#define ADC_DIFF_INPUT1         adcNegSelAPORT3YCH9     /* PA1 negative */
#define ADC_INPUT2              adcPosSelAPORT3XCH2     /* PD10 */
#define ADC_INPUT3              adcPosSelAPORT3YCH3     /* PD11 */
#define ADC_CAL_INPUT           adcPosSelAPORT3XCH8     /* PA0 */
#define ADC_NEG_OFFSET_VALUE    0xfff0                  /* Negative offset calibration */
#define ADC_GAIN_CAL_VALUE      0xffd0                  /* Gain calibration */
#define ADC_PRS_CH_SELECT       adcPRSSELCh0
#define ADC_SINGLE_DVL          4
#define ADC_SCAN_DVL            4
#define ADC_SCAN_DIFF_DVL       2
#define ADC_BUFFER_SIZE         64
#define ADC_VIN_ATT             9                       /* VIN attenuation factor */
#define ADC_VREF_ATT            0                       /* VREF attenuation factor */
#define ADC_SE_VFS              (float)3.3              /* AVDD */
#define ADC_DIFF_VFS            (float)6.6              /* 2xAVDD */
#define ADC_SCALE_VFS           (float)2.2              /* Scaled VFS */
#define ADC_12BIT_MAX           4096                    /* 2^12 */
#define ADC_16BIT_MAX           65536                   /* 2^16 */
#define ADC_CMP_GT_VALUE        3724                    /* ~3.0V for 3.3V AVDD */
#define ADC_CMP_LT_VALUE        620                     /* ~0.5V for 3.3V AVDD */
#define ADC_DMA_CHANNEL         0
#define ADC_DMA_CH_MASK         (1 << ADC_DMA_CHANNEL)

/* Defines for RTCC */
#define RTCC_CC_CHANNEL         1
#define RTCC_PRS_CHANNEL        0                       /* =ADC_PRS_CH_SELECT */
#define RTCC_PRS_CH_SELECT      rtccPRSCh0              /* =ADC_PRS_CH_SELECT */
#define RTCC_WAKEUP_MS          10
#define RTCC_WAKEUP_COUNT       (((32768 * RTCC_WAKEUP_MS) / 1000) - 1)

/* Buffer for ADC interrupt flag */
volatile uint32_t adcIntFlag;

/* Buffer for ADC single and scan conversion */
uint32_t adcBuffer[ADC_BUFFER_SIZE];


void adcSingleScan(bool ovs);

void adcReset(void);

void rtccSetup(void);

void adcSetup(void);

void ldmaSetup(void);




#endif /* ADC_H_ */
