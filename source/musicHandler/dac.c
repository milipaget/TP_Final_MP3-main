/***************************************************************************//**
  @file     dac.c
  @brief    Driver for Digital to Analog Conversion
  @author   TEAM OREO
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "dac.h"

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

// /** Array initializer of DAC peripheral base pointers */
//#define DAC_BASE_PTRS                            { DAC0, DAC1 }

void dacInit (void)
{
	static bool yaInit = false;
    	if (yaInit)
        		return;
	SIM->SCGC2 |= SIM_SCGC2_DAC0_MASK;

	DAC0->C0 = (DAC_C0_DACEN_MASK | DAC_C0_DACTRGSEL_MASK |DAC_C0_DACRFS_MASK); // enable | trigger por software | DACREF2
	//pag 935 c0 tiene para interrupciones del dac										    // para dacref1 poner un 1 con DAC_C0_DACRFS_SHIFT
	yaInit = true;
}


void dacDMAEN(void)
{
	DAC0->C1 = DAC_C1_DMAEN_MASK;
	//pag 936 c1 tiene configs para buffers
}

void dacSendData(uint16_t datatosend)
{

	DAC0->DAT[0].DATH = DAC_DATH_DATA1(datatosend / 256);
	DAC0->DAT[0].DATL = DAC_DATL_DATA0(datatosend % 256);
}

/*! @name DATL - DAC Data Low Register */
/*! @{ */

//#define DAC_DATL_DATA0_MASK                      (0xFFU)
//#define DAC_DATL_DATA0_SHIFT                     (0U)
//#define DAC_DATL_DATA0(x)                        (((uint8_t)(((uint8_t)(x)) << DAC_DATL_DATA0_SHIFT)) & DAC_DATL_DATA0_MASK)
/*! @} */

/*! @name DATH - DAC Data High Register */
/*! @{ */

//#define DAC_DATH_DATA1_MASK                      (0xFU)
//#define DAC_DATH_DATA1_SHIFT                     (0U)
//#define DAC_DATH_DATA1(x)                        (((uint8_t)(((uint8_t)(x)) << DAC_DATH_DATA1_SHIFT)) & DAC_DATH_DATA1_MASK)
/*! @} */

