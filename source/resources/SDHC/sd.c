/***************************************************************************//**
  @file		SD.c
  @author	TEAM OREO
  @date		7/01/2023
 ******************************************************************************/
/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "SD.h"
#include "gpio.h"
#include "MK64F12.h"

#include <stddef.h>

#ifdef SD_DEBUG
#include <stdio.h>
#endif
/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define SD_PORT			PORTE
#define SD_PORT_NUM		PE

#define DAT0_PIN		1
#define DAT1_PIN		0
#define DAT2_PIN		5
#define DAT3_PIN		4

#define CMD_PIN			3
#define CLK_PIN			2

#define SD_DETECT_PIN	6

#define SD_DETECT_GPIO PORTNUM2PIN(SD_PORT_NUM, SD_DETECT_PIN)

#define SD_MUX_ALT		4

#define SD_CARD_PRESENT_STATE	HIGH

#define SD_RESPONSE_NONE		0UL		// No response expected
#define SD_RESPONSE_LONG		1UL		// Long response (136-bit)
#define SD_RESPONSE_SHORT		2UL		// Short response (48-bit)
#define SD_RESPONSE_SHORT_BUSY	3UL		// Short response with busy signal (48-bit)

//#define SD_R1_LENGTH	SD_RESPONSE_SHORT
//#define SD_R1b_LENGTH	SD_RESPONSE_SHORT_BUSY
//#define SD_R2_LENGTH	SD_RESPONSE_LONG
//#define SD_R6_LENGTH	SD_RESPONSE_SHORT

#define OCR_VDD32_33		(1U << 20U)
#define OCR_VDD33_34		(1U << 21U)
#define OCR_BUSY_MASK		(0x80000000U)

#define SDHC_OCR		(OCR_VDD32_33 | OCR_VDD33_34)

#define SDHC_DTW_4BIT		(1U)

#define SDHC_IRQSTAT_ERRORS_MASK	(0x117F0000U)

#define SD_BLKSIZE			(512U)

#define RCA_ARG_SHIFT		(16U)
#define RCA_ARG(rca)		((rca) << RCA_ARG_SHIFT)

#define CMD8_ARG			(0x1AAU)

#define ACMD41_HCS_SHIFT	(30U)
#define ACMD41_HCS(x)		((x) << ACMD41_HCS_SHIFT)
#define ACMD41_XPC_SHIFT	(28U)
#define ACMD41_XPC(x)		((x) << ACMD41_XPC_SHIFT)
#define ACMD41_S18R_SHIFT	(24U)
#define ACMD41_S18R(x)		((x) << ACMD41_S18R_SHIFT)
#define ACMD41_OCR_SHIFT	(0U)
#define ACMD41_OCR(x)		((x) << ACMD41_OCR_SHIFT)
#define ACMD41_ARG(hcs, xpc, s18r, ocr)		(ACMD41_HCS(hcs) | ACMD41_XPC(xpc) | ACMD41_S18R(s18r) | ACMD41_OCR(ocr))

#define ACMD6_4BIT_ARG		(2U)

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef enum {	SDNoResponse,
				SDResponseR1,
				SDResponseR1b,
				SDResponseR2,
				SDResponseR3,
				SDResponseR4,
				SDResponseR5,
				SDResponseR6,
				SDResponseR7,
				SDResponseDataSingle,
				SDResponseDataMulti
				} SD_RESPONSE_TYPE;

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static uint32_t SDSendCmd(uint8_t cmd, uint32_t argument, SD_RESPONSE_TYPE rspIndex, uint32_t* response);

static void isCardCallback();

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static DSTATUS SDState = STA_NOINIT | STA_NODISK;	// Set NOINIT on system reset

static uint16_t rca = 0x0U;		// RCA Address of SD Card

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

bool SDInit() {

	static bool yaInit = false;		// Initialization flag

	if (!yaInit) {
			yaInit = true;
			SD_PORT->PCR[DAT0_PIN] = PORT_PCR_MUX(SD_MUX_ALT) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
			SD_PORT->PCR[DAT1_PIN] = PORT_PCR_MUX(SD_MUX_ALT) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
			SD_PORT->PCR[DAT2_PIN] = PORT_PCR_MUX(SD_MUX_ALT) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
			SD_PORT->PCR[DAT3_PIN] = PORT_PCR_MUX(SD_MUX_ALT) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
			SD_PORT->PCR[CMD_PIN] = PORT_PCR_MUX(SD_MUX_ALT) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
			SD_PORT->PCR[CLK_PIN] = PORT_PCR_MUX(SD_MUX_ALT) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
	

			gpioMode(SD_DETECT_GPIO, INPUT_PULLDOWN);	// para detectar tarjeta
			gpioIRQ(SD_DETECT_GPIO, GPIO_IRQ_MODE_BOTH_EDGES, isCardCallback);

			isCardCallback();  	// Call function to initialize SDState

			// Clock gating
			SIM->SOPT2 &= ~SIM_SOPT2_SDHCSRC_MASK;
			SIM->SCGC3 |= SIM_SCGC3_SDHC_MASK;
	}

	return false;
}

DSTATUS SD_status () {
	return SDState;
}


DSTATUS SD_initial () {

	SDState |= STA_NOINIT;

	SDInit();

	SDHC->SYSCTL |= SDHC_SYSCTL_RSTA_MASK;	// Primero tengo que hacer un reset para inicializar

	SDHC->PROCTL = SDHC_PROCTL_EMODE(2U);		// modo configuracion: littleendian y en 1bit 
	SDHC->IRQSIGEN = 0U;			
	SDHC->VENDOR = 0U;			
	SDHC->BLKATTR = SDHC_BLKATTR_BLKSIZE(SD_BLKSIZE);

	if(isSDCardInserted()) {	// si tengo una tarjeta

		SDHC->SYSCTL = SDHC_SYSCTL_SDCLKFS(0x80) | SDHC_SYSCTL_SDCLKEN_MASK;	// para la etapa de configuración necesito el clock en 400k
		while (SDHC->PRSSTAT & (SDHC_PRSSTAT_CIHB_MASK | SDHC_PRSSTAT_CDIHB_MASK));	// espero a ver si lo pudo lograr

		SDHC->SYSCTL |= SDHC_SYSCTL_INITA_MASK;

		uint32_t res[4], err;
		err = SDSendCmd(0, 0x0, SDNoResponse, NULL);	// inicializo en idle
		if (err) return SDState;

		err = SDSendCmd(8, CMD8_ARG, SDResponseR7, res);	

		if (res[0] != CMD8_ARG || err) {		
			return SDState;
		}

		rca = 0x0U;				
		uint32_t tries = 1000;	// defino un tiempo para esperar la respuesta del comando

		do {
			err = SDSendCmd(55, RCA_ARG(rca), SDResponseR1, res);		// CMD55 para avisar que voy a mandar un appcmd

			if (err) return SDState;

			err = SDSendCmd(41, ACMD41_ARG(1U, 1U, 0U, SDHC_OCR), SDResponseR3, res);		// CMD41: SD_SEND_OP_COND

			if (!(res[0] & SDHC_OCR) || err) {		// no lo logro :(
				return SDState;
			}

			uint32_t cont = 0xFFFFF;
			while (cont--);
		} while (!(res[0] & OCR_BUSY_MASK) && --tries);	 //o se le acaba el tiempo que le di o logra una respuesta

		err = SDSendCmd(2, 0x0U, SDResponseR2, res);

		if (err) return SDState;

		err = SDSendCmd(3, 0x0U, SDResponseR6, res);
		if (err) return SDState;
		rca = res[0] >> RCA_ARG_SHIFT;
		//Si llego aca, es porque logro ponerse en standby para configurar los parametros


		err = SDSendCmd(9, RCA_ARG(rca), SDResponseR2, res);

		if (err) return SDState;

		//Primero aumentamos el clock a 25MHz(es la velocidad maxima de la SD)
		SDHC->SYSCTL &= ~SDHC_SYSCTL_SDCLKEN_MASK;		// apagamos el clock actual de 400k
		SDHC->SYSCTL = SDHC_SYSCTL_SDCLKFS(2U) | SDHC_SYSCTL_DTOCV(0xEU) ;
		while (!(SDHC->PRSSTAT & SDHC_PRSSTAT_SDSTB_MASK));	// espero que sea estable antes de habilitarlo
		SDHC->SYSCTL |= SDHC_SYSCTL_SDCLKEN_MASK;		

		err = SDSendCmd(7, RCA_ARG(rca), SDResponseR1b, res);
		if (err) return SDState;


		// tengo que configurarlo en 4bit
		err = SDSendCmd(55, RCA_ARG(rca), SDResponseR1, res);		
		if (err) return SDState;
		err = SDSendCmd(6, ACMD6_4BIT_ARG, SDResponseR1, res);	
	
		if (err) return SDState;

		SDHC->PROCTL |= SDHC_PROCTL_DTW(SDHC_DTW_4BIT);		

		// le doy un tamaño a los bloques de lectura
		err = SDSendCmd(16, SD_BLKSIZE, SDResponseR1, res);

		if (err) return SDState;

		SDState = 0U;	// si llego aca es que pudo inicializar todo bien
	}
	else {		// si no hubiera tenido una tarjeta puesta
		SDState |= STA_NODISK;
	}

	return SDState;
}

DRESULT SD_read (
  BYTE* buff,    /* [OUT] Pointer to the read data buffer */
  LBA_t sector,  /* [IN] Start sector number */
  UINT count     /* [IN] Number of sectors to read */
) {
//Para la lectura tengo dos modos, de a un bloque o multibloques y los comandos son diferentes
	if (SDState & STA_NOINIT) return RES_NOTRDY;

	uint32_t res, err = 0xFFFFFFFF;

	UINT index = 0U;
	uint8_t rdwml = SDHC->WML & SDHC_WML_RDWML_MASK;

	if (count == 1) {
		// para un solito es 17

		err = SDSendCmd(17, sector, SDResponseDataSingle, &res);
		if (err) return RES_ERROR;

	}
	else {
		// para muchos es 18
		SDHC->BLKATTR = SDHC_BLKATTR_BLKCNT(count) | SDHC_BLKATTR_BLKSIZE(SD_BLKSIZE);
		err = SDSendCmd(18, sector, SDResponseDataMulti, &res);

		if (err) return RES_ERROR;
	}


	while (!(SDHC->IRQSTAT & SDHC_IRQSTAT_TC_MASK)) {
		if (SDHC->PRSSTAT & SDHC_PRSSTAT_BREN_MASK) {
			for (int i = 0; i < rdwml; i++) {
				((uint32_t*)buff)[index] = SDHC->DATPORT;
				index++;
			}
		}
	}

	err = SDHC->IRQSTAT & SDHC_IRQSTAT_ERRORS_MASK;
	SDHC->IRQSTAT |= SDHC_IRQSTAT_TC_MASK;		// tengo que limpiar las flag de interrupciones para que se puedan activar

	return err ? RES_ERROR : RES_OK;
}

bool isSDCardInserted() {
	return !(SDState & STA_NODISK);
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
static uint32_t SDSendCmd(uint8_t cmd, uint32_t argument, SD_RESPONSE_TYPE rspIndex, uint32_t* response) {
	//tengo que mandar los comandos que le voy a pasando
	uint32_t err = 0xFFFFFFFF;
	uint32_t xferType = SDHC_XFERTYP_CMDINX(cmd);

	uint8_t rspLength;

	switch (rspIndex) {

		case SDNoResponse:
			xferType |= SDHC_XFERTYP_RSPTYP(SD_RESPONSE_NONE);
			rspLength = SD_RESPONSE_NONE;
			break;

		case SDResponseR1:
		case SDResponseR5:
		case SDResponseR6:
		case SDResponseR7:
			xferType |= SDHC_XFERTYP_RSPTYP(SD_RESPONSE_SHORT) | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_CICEN_MASK;
			rspLength = SD_RESPONSE_SHORT;
			break;

		case SDResponseR1b:
			xferType |= SDHC_XFERTYP_RSPTYP(SD_RESPONSE_SHORT_BUSY) | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_CICEN_MASK;
			rspLength = SD_RESPONSE_SHORT;
			break;

		case SDResponseR2:
			xferType |= SDHC_XFERTYP_RSPTYP(SD_RESPONSE_LONG) | SDHC_XFERTYP_CCCEN_MASK;
			rspLength = SD_RESPONSE_LONG;
			break;

		case SDResponseR3:
		case SDResponseR4:
			xferType |= SDHC_XFERTYP_RSPTYP(SD_RESPONSE_SHORT);
			rspLength = SD_RESPONSE_SHORT;
			break;

		case SDResponseDataSingle:
			xferType |= SDHC_XFERTYP_RSPTYP(SD_RESPONSE_SHORT) | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_CICEN_MASK |
						SDHC_XFERTYP_DPSEL_MASK | SDHC_XFERTYP_DTDSEL_MASK;
			rspLength = SD_RESPONSE_SHORT;
			break;

		case SDResponseDataMulti:
			xferType |= SDHC_XFERTYP_RSPTYP(SD_RESPONSE_SHORT) | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_CICEN_MASK |
						SDHC_XFERTYP_DPSEL_MASK | SDHC_XFERTYP_DTDSEL_MASK |
						SDHC_XFERTYP_BCEN_MASK | SDHC_XFERTYP_MSBSEL_MASK | SDHC_XFERTYP_AC12EN_MASK;
			rspLength = SD_RESPONSE_SHORT;
			break;

		default:
			return err;

	}

	if (!(SDHC->PRSSTAT & (SDHC_PRSSTAT_CIHB_MASK | SDHC_PRSSTAT_CDIHB_MASK | SDHC_PRSSTAT_WTA_MASK))) {

		SDHC->CMDARG = argument;
		SDHC->XFERTYP = xferType;

		while(!(SDHC->IRQSTAT & SDHC_IRQSTAT_CC_MASK));	

		err = SDHC->IRQSTAT & SDHC_IRQSTAT_ERRORS_MASK;
		SDHC->IRQSTAT |= err;
		err &= 0xFFFFFFFE;

		if (rspLength != SD_RESPONSE_NONE && response != NULL) {
			response[0] = SDHC->CMDRSP[0];
			if (rspLength == SD_RESPONSE_LONG) {	// Long response
				response[1] = SDHC->CMDRSP[1];
				response[2] = SDHC->CMDRSP[2];
				response[3] = SDHC->CMDRSP[3];
			}
		}


	}

	return err;

}


static void isCardCallback() {

	if (gpioRead(SD_DETECT_GPIO) == SD_CARD_PRESENT_STATE) {
		SDState = STA_NOINIT;
	}
	else {
		SDState = STA_NOINIT | STA_NODISK;
	}

}
