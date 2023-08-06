/*
 * TMC5160.c
 *
 *  Created on: 04.07.2017
 *      Author: LK based on ED based on BS
 */

#include "TMC5160.h"
#include "string.h"
#include "gpioDecal.h"
#include "board.h"

// todo ADD API 3: Some Registers have default hardware configuration from OTP, we shouldnt overwrite those - add the weak write register permission and use it here (LH)
// Default Register Values
#define R00 0x00000008  // GCONF
#define R09 0x00010606  // SHORT_CONF
#define R0A 0x00080400  // DRV_CONF
#define R10 0x00070A03  // IHOLD_IRUN
#define R11 0x0000000A  // TPOWERDOWN
#define R2B 0x00000001  // VSTOP
#define R3A 0x00010000  // ENC_CONST
#define R60 0xAAAAB554  // MSLUT[0]
#define R61 0x4A9554AA  // MSLUT[1]
#define R62 0x24492929  // MSLUT[2]
#define R63 0x10104222  // MSLUT[3]
#define R64 0xFBFFFFFF  // MSLUT[4]
#define R65 0xB5BB777D  // MSLUT[5]
#define R66 0x49295556  // MSLUT[6]
#define R67 0x00404222  // MSLUT[7]
#define R68 0xFFFF8056  // MSLUTSEL
#define R69 0x00F70000  // MSLUTSTART
#define R6C 0x00410153  // CHOPCONF
#define R70 0xC40C001E  // PWMCONF

/* Register access permissions:
 * 0: none (reserved)
 * 1: read
 * 2: write
 * 3: read/write
 * 7: read^write (seperate functions/values)
 */
const uint8_t tmc5160_defaultRegisterAccess[TMC5160_REGISTER_COUNT] =
{
//	0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
	3, 7, 1, 2, 7, 2, 2, 1, 1, 2, 2, 2, 1, 0, 0, 0, // 0x00 - 0x0F
	2, 2, 1, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x10 - 0x1F
	3, 3, 1, 2, 2, 2, 2, 2, 2, 0, 2, 2, 2, 3, 0, 0, // 0x20 - 0x2F
	0, 0, 0, 2, 3, 7, 1, 0, 3, 3, 2, 7, 1, 2, 0, 0, // 0x30 - 0x3F
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x40 - 0x4F
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x50 - 0x5F
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 3, 2, 2, 1, // 0x60 - 0x6F
	2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  // 0x70 - 0x7F
};

const int32_t tmc5160_defaultRegisterResetState[TMC5160_REGISTER_COUNT] =
{
//	0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
	R00, 0,   0,   0,   0,   0,   0,   0,   0,   R09, R0A, 0,   0,   0,   0,   0, // 0x00 - 0x0F
	R10, R11, 0,   0,   0,   0,   0,   0,   0,   0,   0,   R2B, 0,   0,   0,   0, // 0x10 - 0x1F
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 0x20 - 0x2F
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   R3A, 0,   0,   0,   0,   0, // 0x30 - 0x3F
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 0x40 - 0x4F
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 0x50 - 0x5F
	R60, R61, R62, R63, R64, R65, R66, R67, R68, R69, 0,   0,   R6C, 0,   0,   0, // 0x60 - 0x6F
	R70, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0  // 0x70 - 0x7F
};

static uint32_t tmc5160_right(TMC5160_RSRC *rsrc, int32_t velocity);
static uint32_t tmc5160_left(TMC5160_RSRC *rsrc, int32_t velocity);
static uint32_t tmc5160_rotate(TMC5160_RSRC *rsrc, int32_t velocity);
static uint32_t tmc5160_stop(TMC5160_RSRC *rsrc);
static uint32_t tmc5160_moveTo(TMC5160_RSRC *rsrc, int32_t position);
static uint32_t tmc5160_moveBy(TMC5160_RSRC *rsrc, int32_t *ticks);
//static uint32_t tmc5160_GAP(TMC5160_RSRC *rsrc, uint8_t type, int32_t *value);
//static uint32_t tmc5160_SAP(TMC5160_RSRC *rsrc, uint8_t type, int32_t value);
static void tmc5160_readRegister(TMC5160_RSRC *rsrc, uint8_t address, int32_t *value);
static void tmc5160_writeRegister(TMC5160_RSRC *rsrc, uint8_t address, int32_t value);
static uint32_t tmc5160_getMeasuredSpeed(TMC5160_RSRC *rsrc, int32_t *value);

static void tmc5160_periodicJob(TMC5160_RSRC *rsrc, uint32_t tick);
static uint8_t tmc5160_reset(TMC5160_RSRC *rsrc);
static void tmc5160_initConfig(TMC5160_RSRC *rsrc);
static void TMC5160_init(TMC5160_RSRC *rsrc);
static void tmc5160_deInit(TMC5160_RSRC *rsrc);

// lower io
static void tmc5160_writeDatagram(TMC5160_RSRC *rsrc, uint8_t address, uint8_t x1, uint8_t x2, uint8_t x3, uint8_t x4);
static void tmc5160_writeInt(TMC5160_RSRC *rsrc, uint8_t address, int value);
static int tmc5160_readInt(TMC5160_RSRC *rsrc, uint8_t address);

void setupDev_tmc5160(
	TMC5160_DEV *dev,
	const char* NAME,
	SPI_HandleTypeDef* SPI_HANDLE,	// spi handle
	const PIN_T *CS,				// spi select
	const PIN_T *DRV_ENN_CFG6,		// driver enable
	const PIN_T *SD_MODE,			//
	const PIN_T *SWN_DIAG0,			//
	const PIN_T *SWP_DIAG1,			//
	const PIN_T *ENCA_DCIN_CFG5,	//
	const PIN_T *ENCB_DCEN_CFG4,	//
	const PIN_T *ENCN_DCO			//
){
	memset(dev, 0, sizeof(TMC5160_DEV));
	strcpy(dev->rsrc.name, NAME);
	dev->rsrc.SPI_HANDLE = SPI_HANDLE;
	dev->rsrc.CS = CS;
	dev->rsrc.DRV_ENN_CFG6 = DRV_ENN_CFG6;
	dev->rsrc.SD_MODE = SD_MODE;
	dev->rsrc.SWN_DIAG0 = SWN_DIAG0;
	dev->rsrc.SWP_DIAG1 = SWP_DIAG1;
	dev->rsrc.ENCA_DCIN_CFG5 = ENCA_DCIN_CFG5;
	dev->rsrc.ENCB_DCEN_CFG4 = ENCB_DCEN_CFG4;
	dev->rsrc.ENCN_DCO = ENCN_DCO;

	as_OUTPUT_PP_NOPULL_HIGH(*dev->rsrc.CS);
	as_OUTPUT_PP_NOPULL_HIGH(*dev->rsrc.DRV_ENN_CFG6);
	as_OUTPUT_PP_NOPULL_HIGH(*dev->rsrc.SD_MODE);
	as_INPUT_PULLUP(*dev->rsrc.SWN_DIAG0);
	as_INPUT_PULLUP(*dev->rsrc.SWP_DIAG1);
	as_INPUT_NOPULL(*dev->rsrc.ENCA_DCIN_CFG5);
	as_INPUT_NOPULL(*dev->rsrc.ENCB_DCEN_CFG4);
	as_INPUT_NOPULL(*dev->rsrc.ENCN_DCO);

	tmc5160_initConfig(&dev->rsrc);

	// reg funtions
	dev->right = tmc5160_right;
	dev->left = tmc5160_left;
	dev->rotate = tmc5160_rotate;
	dev->stop = tmc5160_stop;
	dev->moveTo = tmc5160_moveTo;
	dev->moveBy = tmc5160_moveBy;
//	dev->GAP = tmc5160_GAP;
//	dev->SAP = tmc5160_SAP;
	dev->readRegister = tmc5160_readRegister;
	dev->writeRegister = tmc5160_writeRegister;
	dev->getMeasuredSpeed = tmc5160_getMeasuredSpeed;
	dev->periodicJob = tmc5160_periodicJob;
	dev->init = TMC5160_init;
	dev->deInit = tmc5160_deInit;
	dev->reset = tmc5160_reset;
}

static uint32_t tmc5160_rotate(TMC5160_RSRC *rsrc, int32_t velocity)
{
	HAL_GPIO_WritePin(rsrc->DRV_ENN_CFG6->GPIOx, rsrc->DRV_ENN_CFG6->GPIO_Pin, GPIO_PIN_RESET);
	rsrc->vMaxModified = true;

	// set absolute velocity, independant from direction
	tmc5160_writeInt(rsrc, TMC5160_VMAX, abs(velocity));

	// signdedness defines velocity mode direction bit in rampmode register
	tmc5160_writeDatagram(rsrc, TMC5160_RAMPMODE, 0, 0, 0, (velocity >= 0)? 1 : 2);

	return TMC_ERROR_NONE;
}

static uint32_t tmc5160_right(TMC5160_RSRC *rsrc, int32_t velocity)
{
	return tmc5160_rotate(rsrc, velocity);
}

static uint32_t tmc5160_left(TMC5160_RSRC *rsrc, int32_t velocity)
{
	return tmc5160_rotate(rsrc, -velocity);
}

static uint32_t tmc5160_stop(TMC5160_RSRC *rsrc)
{
	return tmc5160_rotate(rsrc, 0);
}

static uint32_t tmc5160_moveTo(TMC5160_RSRC *rsrc, int32_t position)
{
	HAL_GPIO_WritePin(rsrc->DRV_ENN_CFG6->GPIOx, rsrc->DRV_ENN_CFG6->GPIO_Pin, GPIO_PIN_RESET);
	if(rsrc->vMaxModified)
	{
		tmc5160_writeInt(rsrc, TMC5160_VMAX, rsrc->TMC5160.regShadow[TMC5160_VMAX]);
		rsrc->vMaxModified = false;
	}

	// set position
	tmc5160_writeInt(rsrc, TMC5160_XTARGET, position);

	// change to positioning mode
	tmc5160_writeDatagram(rsrc, TMC5160_RAMPMODE, 0, 0, 0, 0);

	return TMC_ERROR_NONE;
}

static uint32_t tmc5160_moveBy(TMC5160_RSRC *rsrc, int32_t *ticks)
{
	// determine actual position and add numbers of ticks to move
	*ticks = tmc5160_readInt(rsrc, TMC5160_XACTUAL) + *ticks;

	return tmc5160_moveTo(rsrc, *ticks);
}


static void tmc5160_writeRegister(TMC5160_RSRC *rsrc, uint8_t address, int32_t value)
{
	tmc5160_writeInt(rsrc, address, value);
}

static void tmc5160_readRegister(TMC5160_RSRC *rsrc, uint8_t address, int32_t *value)
{
	*value = tmc5160_readInt(rsrc, address);
}

//static uint32_t tmc5160_GAP(TMC5160_RSRC *rsrc, uint8_t type, int32_t *value)
//{
//	return 0;
//}
//static uint32_t tmc5160_SAP(TMC5160_RSRC *rsrc, uint8_t type, int32_t value){
//	return 0;
//}

static uint32_t tmc5160_getMeasuredSpeed(TMC5160_RSRC *rsrc, int32_t *value)
{
	*value = rsrc->TMC5160.velocity;
	return TMC_ERROR_NONE;
}


static void tmc5160_writeDatagram(TMC5160_RSRC *rsrc, uint8_t address, uint8_t x1, uint8_t x2, uint8_t x3, uint8_t x4)
{
	u8 txBuf[5], rxBuf[5]={0};
	address = TMC_ADDRESS(address);

	txBuf[0] = address|0x80;
	txBuf[1] = x1;
	txBuf[2] = x2;
	txBuf[3] = x3;
	txBuf[4] = x4;

	while (HAL_SPI_GetState(rsrc->SPI_HANDLE) != HAL_SPI_STATE_READY){}
	/* nSS signal activation - low */
	HAL_GPIO_WritePin(rsrc->CS->GPIOx, rsrc->CS->GPIO_Pin, GPIO_PIN_RESET);
	/* SPI byte send */
	HAL_SPI_TransmitReceive(rsrc->SPI_HANDLE, txBuf, rxBuf, 5, 2);
	/* nSS signal deactivation - high */
	HAL_GPIO_WritePin(rsrc->CS->GPIOx, rsrc->CS->GPIO_Pin, GPIO_PIN_SET);

	int value = x1;
	value <<= 8;
	value |= x2;
	value <<= 8;
	value |= x3;
	value <<= 8;
	value |= x4;

	rsrc->TMC5160.regShadow[address] = value;
}

static void tmc5160_writeInt(TMC5160_RSRC *rsrc, uint8_t address, int value)
{
	tmc5160_writeDatagram(rsrc, address, 0xFF & (value>>24), 0xFF & (value>>16), 0xFF & (value>>8), 0xFF & (value>>0));
}

static int tmc5160_readInt(TMC5160_RSRC *rsrc, uint8_t address)
{
	u8 txBuf[5], rxBuf[5]={0};
	int value = 0;
	address = TMC_ADDRESS(address);

	// Register not readable -> shadow register copy
	if(!TMC_IS_READABLE(rsrc->TMC5160.registerAccess[address]))
		return rsrc->TMC5160.regShadow[address];

	txBuf[0] = address;
	txBuf[1] = 0;
	txBuf[2] = 0;
	txBuf[3] = 0;
	txBuf[4] = 0;

	while (HAL_SPI_GetState(rsrc->SPI_HANDLE) != HAL_SPI_STATE_READY);
	/* nSS signal activation - low */
	HAL_GPIO_WritePin(rsrc->CS->GPIOx, rsrc->CS->GPIO_Pin, GPIO_PIN_RESET);
	/* SPI byte send */
	HAL_SPI_TransmitReceive(rsrc->SPI_HANDLE, txBuf, rxBuf, 5, 2);
	/* nSS signal deactivation - high */
	HAL_GPIO_WritePin(rsrc->CS->GPIOx, rsrc->CS->GPIO_Pin, GPIO_PIN_SET);

	miscDelay(10);
	memset(rxBuf, 0, 5);

	while (HAL_SPI_GetState(rsrc->SPI_HANDLE) != HAL_SPI_STATE_READY);
	/* nSS signal activation - low */
	HAL_GPIO_WritePin(rsrc->CS->GPIOx, rsrc->CS->GPIO_Pin, GPIO_PIN_RESET);
	/* SPI byte send */
	HAL_SPI_TransmitReceive(rsrc->SPI_HANDLE, txBuf, rxBuf, 5, 2);
	/* nSS signal deactivation - high */
	HAL_GPIO_WritePin(rsrc->CS->GPIOx, rsrc->CS->GPIO_Pin, GPIO_PIN_SET);

	value = rxBuf[1];	value <<= 8;
	value |= rxBuf[2];	value <<= 8;
	value |= rxBuf[3];	value <<= 8;
	value |= rxBuf[4];

	return value;
}

static void tmc5160_initConfig(TMC5160_RSRC *rsrc)
{
	TMC5160TypeDef *tmc5160 = & rsrc->TMC5160;
	tmc5160->velocity  = 0;
	tmc5160->oldTick   = 0;
	tmc5160->oldX      = 0;

	int i;
	for(i = 0; i < TMC5160_REGISTER_COUNT; i++)
	{
		tmc5160->registerAccess[i]      = tmc5160_defaultRegisterAccess[i];
		tmc5160->regShadow[i]  = tmc5160_defaultRegisterResetState[i];
	}
}

//static void tmc5160_writeConfiguration(TMC5160_RSRC *rsrc)
//{
//	TMC5160TypeDef *tmc5160 = &rsrc->TMC5160;
////	ConfigurationTypeDef *TMC5160_config = rsrc->TMC5160_config;
//
//	uint8_t *ptr = &TMC5160_config->configIndex;
//	const int32_t *settings = (TMC5160_config->state == CONFIG_RESTORE) ? TMC5160_config->shadowRegister : tmc5160->registerResetState;
//
//	while((*ptr < TMC5160_REGISTER_COUNT) && !TMC_IS_WRITABLE(tmc5160->registerAccess[*ptr]))
//		(*ptr)++;
//
//	if(*ptr < TMC5160_REGISTER_COUNT)
//	{
//		tmc5160_writeInt(rsrc, *ptr, settings[*ptr]);
//		(*ptr)++;
//	}
//	else
//	{
//		TMC5160_config->state = CONFIG_READY;
//	}
//}

static void tmc5160_periodicJob(TMC5160_RSRC *rsrc, uint32_t tick)
{
	int XActual;
	rsrc->tick += tick;
	if(rsrc->tick >= 5) // measured speed dx/dt
	{
		XActual = tmc5160_readInt(rsrc, TMC5160_XACTUAL);
		rsrc->TMC5160.velocity = (int) ((float) ((XActual-rsrc->TMC5160.oldX) / (float) rsrc->tick) * (float) 1048.576);
		rsrc->TMC5160.oldX     = XActual;
		rsrc->TMC5160.oldTick  = tick;
		rsrc->tick = 0;
	}
}

static uint8_t tmc5160_reset(TMC5160_RSRC *rsrc)
{
	return 1;
}

static void TMC5160_init(TMC5160_RSRC *rsrc)
{
	u16 i;
	tmc5160_initConfig(rsrc);
	for(i=0; i < TMC5160_REGISTER_COUNT; i++)
	{
		tmc5160_writeInt(rsrc, i, rsrc->TMC5160.regShadow[i]);
	}
};

static void tmc5160_deInit(TMC5160_RSRC *rsrc){
	if(rsrc->DRV_ENN_CFG6)
		HAL_GPIO_WritePin(rsrc->DRV_ENN_CFG6->GPIOx, rsrc->DRV_ENN_CFG6->GPIO_Pin, GPIO_PIN_RESET);
	if(rsrc->SD_MODE)
		HAL_GPIO_WritePin(rsrc->SD_MODE->GPIOx, rsrc->SD_MODE->GPIO_Pin, GPIO_PIN_RESET);
	if(rsrc->SPI_MODE)
		HAL_GPIO_WritePin(rsrc->SPI_MODE->GPIOx, rsrc->SPI_MODE->GPIO_Pin, GPIO_PIN_RESET);
}

