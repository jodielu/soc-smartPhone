/*
 * adc.c
 *
 *  Created on: Jul 17, 2018
 *      Author: memsj
 */

#include "adc.h"
#include "app_ui.h"
#include "htm.h"

#define ADC_VALUE_TEXT 							"Single PA0:\n %5luV\n"
#define TEST_VALUE_TEXT							"Testing: %d\n"

float adc_value;
/**************************************************************************//**
 * @brief Setup RTCC as PRS source to trigger ADC
 *****************************************************************************/
void rtccSetup(void)
{
  RTCC_Init_TypeDef rtccInit = RTCC_INIT_DEFAULT;
  RTCC_CCChConf_TypeDef rtccInitCompareChannel = RTCC_CH_INIT_COMPARE_DEFAULT;

  /* Enabling clock to the interface of the low energy modules */
  CMU_ClockEnable(cmuClock_CORELE, true);

  /* Routing the LFXO clock to the RTCC */
  CMU_ClockSelectSet(cmuClock_LFE, cmuSelect_LFXO);
  CMU_ClockEnable(cmuClock_RTCC, true);

  rtccInitCompareChannel.prsSel = RTCC_PRS_CH_SELECT;

  /* Setting the compare value of the RTCC */
  RTCC_ChannelInit(RTCC_CC_CHANNEL, &rtccInitCompareChannel);
  RTCC_ChannelCCVSet(RTCC_CC_CHANNEL, RTCC_WAKEUP_COUNT);

  /* Clear counter on compare match */
  rtccInit.cntWrapOnCCV1 = false;
  rtccInit.presc = rtccCntPresc_1;
  rtccInit.enable = true;

  rtccInit.debugRun              = false;
  rtccInit.precntWrapOnCCV0      = false;
  rtccInit.prescMode             = rtccCntTickPresc;
  rtccInit.enaOSCFailDetect      = false;
  rtccInit.cntMode               = rtccCntModeNormal;

  /* Initialize the RTCC */
  RTCC_Init(&rtccInit);
}

/**************************************************************************//**
 * @brief Initialize ADC for single and scan conversion
 *****************************************************************************/
void adcSetup(void)
{
  /* Enable ADC clock */
  CMU_ClockEnable(cmuClock_ADC0, true);

  /* Select AUXHFRCO for ADC ASYNC mode so that ADC can run on EM2 */
  CMU->ADCCTRL = CMU_ADCCTRL_ADC0CLKSEL_AUXHFRCO;

  /* Initialize compare threshold for both single and scan conversion */
  ADC0->CMPTHR = _ADC_CMPTHR_RESETVALUE;
  ADC0->CMPTHR = (ADC_CMP_GT_VALUE << _ADC_CMPTHR_ADGT_SHIFT) +
                 (ADC_CMP_LT_VALUE << _ADC_CMPTHR_ADLT_SHIFT);
}

/***************************************************************************//**
 * @brief Initialize the LDMA controller.
 ******************************************************************************/
void ldmaSetup(void)
{
  LDMA_Init_t init = LDMA_INIT_DEFAULT;

  LDMA_Init(&init);
  CMU_ClockEnable(cmuClock_LDMA, false);
}

/**************************************************************************//**
 * @brief ADC single and scan conversion example (Single-ended mode)
 * @param[in] ovs
 *   False for 12 bit ADC, True for 16 bit oversampling ADC.
 *****************************************************************************/
void adcSingleScan(bool ovs)
{
/* Disable key interrupt during run */
NVIC_DisableIRQ(GPIO_EVEN_IRQn);
NVIC_DisableIRQ(GPIO_ODD_IRQn);


  //uint32_t id;
  uint32_t sample, adcMax;
  //uint32_t test;

  ADC_Init_TypeDef init = ADC_INIT_DEFAULT;
  ADC_InitSingle_TypeDef singleInit = ADC_INITSINGLE_DEFAULT;
  ADC_InitScan_TypeDef scanInit = ADC_INITSCAN_DEFAULT;

  adcMax = ADC_12BIT_MAX;
  if (ovs)
  {
    adcMax = ADC_16BIT_MAX;
  }


  /* Init common issues for both single conversion and scan mode */
  init.timebase = ADC_TimebaseCalc(0);
  init.prescale = ADC_PrescaleCalc(ADC_CLOCK, 0);
  if (ovs)
  {
    /* Set oversampling rate */
    init.ovsRateSel = adcOvsRateSel256;
  }
  ADC_Init(ADC0, &init);

  /* Initialize for single conversion */
  singleInit.reference = adcRefVDD;
  singleInit.posSel = ADC_INPUT0;
  singleInit.negSel = adcNegSelVSS;
  if (ovs)
  {
    /* Enable oversampling rate */
    singleInit.resolution = adcResOVS;
  }
  ADC_InitSingle(ADC0, &singleInit);
#ifdef TEST
  /* Setup scan channels, define DEBUG_EFM in debug build to identify invalid channel range */
  ADC_ScanSingleEndedInputAdd(&scanInit, adcScanInputGroup0, ADC_INPUT0);
  ADC_ScanSingleEndedInputAdd(&scanInit, adcScanInputGroup0, ADC_INPUT1);
  ADC_ScanSingleEndedInputAdd(&scanInit, adcScanInputGroup1, ADC_INPUT2);
  ADC_ScanSingleEndedInputAdd(&scanInit, adcScanInputGroup1, ADC_INPUT3);

  /* Initialize for scan conversion */
  scanInit.reference = adcRefVDD;
  if (ovs)
  {
    /* Enable oversampling rate */
    scanInit.resolution = adcResOVS;
  }
  ADC_InitScan(ADC0, &scanInit);
#endif

  /* Set scan data valid level to trigger */
  ADC0->SCANCTRLX |= (ADC_SCAN_DVL - 1) << _ADC_SCANCTRLX_DVL_SHIFT;

  /* Start ADC single conversion */
  ADC_Start(ADC0, adcStartSingle);
  while ((ADC0->IF & ADC_IF_SINGLE) == 0)
    ;

  /* Get ADC single result */
  sample = ADC_DataSingleGet(ADC0);
  adc_value = ((float)sample * ADC_SE_VFS)/adcMax;
  getADCValue(sample);
  //printf("Single PA0: %1.4fV\n",((float)sample * ADC_SE_VFS)/adcMax);

#ifdef TEST
  /* Start ADC scan conversion */
  ADC_Start(ADC0, adcStartScan);
  while ((ADC0->IF & ADC_IF_SCAN) == 0)
    ;


  /* Get ADC scan results */
  sample = ADC_DataIdScanGet(ADC0, &id);
  printf("Scan PA%lu: %1.4fV\n", id, ((float)(sample) * ADC_SE_VFS)/adcMax);
  sample = ADC_DataIdScanGet(ADC0, &id);
  printf("Scan PA%lu: %1.4fV\n", id, ((float)(sample) * ADC_SE_VFS)/adcMax);
  sample = ADC_DataIdScanGet(ADC0, &id);
  printf("Scan PD%lu: %1.4fV\n", id, ((float)(sample) * ADC_SE_VFS)/adcMax);
  sample = ADC_DataIdScanGet(ADC0, &id);
  printf("Scan PD%lu: %1.4fV\n", id, ((float)(sample) * ADC_SE_VFS)/adcMax);
#endif


  //adcReset();
}

/**************************************************************************//**
 * @brief Reset ADC related registers and parameters to default values.
 *****************************************************************************/
#ifdef BUTTONS
void adcReset(void)
{
  uint32_t i;

  /* Switch the ADCCLKMODE to SYNC */
  NVIC_DisableIRQ(ADC0_IRQn);
  ADC0->CTRL &= ~ADC_CTRL_ADCCLKMODE_ASYNC;

  /* Rest ADC registers */
  ADC_Reset(ADC0);

  /* Fill buffer and clear flag */
  for (i=0; i<ADC_BUFFER_SIZE; i++)
  {
    adcBuffer[i] = ADC_CMP_LT_VALUE;
  }
  adcIntFlag = 0;

  /* Reset AUXHFRCO to default */
  CMU_AUXHFRCOFreqSet(cmuAUXHFRCOFreq_19M0Hz);

  /* Disable clock */
  CMU_ClockEnable(cmuClock_PRS, false);
  CMU_ClockEnable(cmuClock_LDMA, false);


  /* Enable key interrupt */
  runKey = false;
  menuKey = false;
  GPIO->IFC = _GPIO_IFC_MASK;
  NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);
  NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
  NVIC_EnableIRQ(GPIO_ODD_IRQn);
}
#endif

/**************************************************************************//**
 * @brief ADC single conversion interrupt example
 * @param[in] emode2
 *   False to run ADC in EM1, True to run ADC in EM2.
 *****************************************************************************/
#ifdef SingleInt
void adcSingleInt(bool emode2)
{
  //uint32_t i;

  ADC_Init_TypeDef init = ADC_INIT_DEFAULT;
  ADC_InitSingle_TypeDef singleInit = ADC_INITSINGLE_DEFAULT;

  /* Initialize for single conversion */
  singleInit.prsSel = ADC_PRS_CH_SELECT;
  singleInit.reference = adcRefVDD;
  singleInit.posSel = ADC_INPUT0;
  singleInit.negSel = adcNegSelVSS;
  singleInit.prsEnable = true;
  singleInit.fifoOverwrite = true;
  ADC_InitSingle(ADC0, &singleInit);

  /* Enable single window compare */
  ADC0->SINGLECTRL |= ADC_SINGLECTRL_CMPEN;

  /* Set single data valid level (DVL) to trigger */
  ADC0->SINGLECTRLX |= (ADC_SINGLE_DVL - 1) << _ADC_SINGLECTRLX_DVL_SHIFT;

  /* Enable ADC Interrupt when reaching DVL and window compare */
  ADC_IntEnable(ADC0, ADC_IEN_SINGLE + ADC_IEN_SINGLECMP);

  if (emode2)
  {
    /* Use LOWACC if not using bandgap reference to reduce current consumption */
    ADC0->BIASPROG = ADC_BIASPROG_GPBIASACC;

    /* Switch the ADCCLKMODE to ASYNC at the end of initialization */
    init.em2ClockConfig = adcEm2ClockOnDemand;

    /* Use AUXHFRCO frequency to setup ADC if run on EM2 */
    CMU_AUXHFRCOFreqSet(ADC_ASYNC_CLOCK);
    init.timebase = ADC_TimebaseCalc(CMU_AUXHFRCOBandGet());
    init.prescale = ADC_PrescaleCalc(ADC_CLOCK, CMU_AUXHFRCOBandGet());
  }
  else
  {
    /* Use HFPERCLK frequency to setup ADC if run on EM1 */
    init.timebase = ADC_TimebaseCalc(0);
    init.prescale = ADC_PrescaleCalc(ADC_CLOCK, 0);
  }
  /* Init common issues for both single conversion and scan mode */
  ADC_Init(ADC0, &init);

#ifdef TEST
  /* Clear the FIFOs and pending interrupt */
  ADC0->SINGLEFIFOCLEAR = ADC_SINGLEFIFOCLEAR_SINGLEFIFOCLEAR;
  NVIC_ClearPendingIRQ(ADC0_IRQn);
  NVIC_EnableIRQ(ADC0_IRQn);
#endif

  /* Start PRS from RTCC */
  RTCC_Enable(true);

  while(1)
  {
    if (emode2)
    {
      EMU_EnterEM2(false);
    }
    else
    {
      EMU_EnterEM1();
    }
    if (adcIntFlag & ADC_IF_SINGLECMP)
    {
      /* Exit if ADC outside the compare window */
      break;
    }
  }

  if (emode2)
  {
	   int tester;
	    tester = 1;
	    char testing[32];
	    snprintf(testing, sizeof(TEST_VALUE_TEXT),TEST_VALUE_TEXT, tester);
	    appUiWriteString(testing);

#ifdef TEST
    /* Enable GPIO clock and power on LCD display */
    CMU_ClockEnable(PAL_SPI_USART_CLOCK, true);
    CMU_ClockEnable(cmuClock_GPIO, true);
    displayDevice.pDisplayPowerOn(&displayDevice, true);
    printf("\f");
    printf("Example %d\n", menuLevel+1);
    printf("ADC Single Conversion\n");
    printf("Interrupt (EM2) - Run\n\n");
#endif
  }

  /* Print ADC value after SINGLECMP interrrupt */
  if (ADC0->SINGLEFIFOCOUNT == 0)
  {

#ifdef TEST
    printf("FIFO 0: %1.4fV\n", ((float)(adcBuffer[0]) * ADC_SE_VFS)/ADC_12BIT_MAX);
    printf("FIFO 1: %1.4fV\n", ((float)(adcBuffer[1]) * ADC_SE_VFS)/ADC_12BIT_MAX);
    printf("FIFO 2: %1.4fV\n", ((float)(adcBuffer[2]) * ADC_SE_VFS)/ADC_12BIT_MAX);
    printf("FIFO 3: %1.4fV\n", ((float)(adcBuffer[3]) * ADC_SE_VFS)/ADC_12BIT_MAX);
  }
  else
  {
    i = 0;
    while (ADC0->SINGLEFIFOCOUNT)
    {
      printf("FIFO %lu: %1.4fV\n", i, ((float)(ADC_DataSingleGet(ADC0)) * ADC_SE_VFS)/ADC_12BIT_MAX);
      i++;
    }
#endif
  }
   int test2;
   test2 = 2;
   char t[32];
   snprintf(t, sizeof(TEST_VALUE_TEXT),TEST_VALUE_TEXT, test2);
   appUiWriteString(t);

  //adcReset();
}
#endif


