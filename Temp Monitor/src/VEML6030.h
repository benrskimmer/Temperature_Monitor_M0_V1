/*
 * VEML6030.h
 *
 * Created: 11/19/2021 12:26:58 AM
 *  Author: Ben
 */ 


#ifndef VEML6030_H_
#define VEML6030_H_

#include <c_types.h>
#include "i2c_master.h"

#define ENABLE        0x01
#define DISABLE       0x00
#define SHUTDOWN      0x01
#define POWER         0x00
#define NO_INT        0x00
#define INT_HIGH      0x01
#define INT_LOW       0x02
#define UNKNOWN_ERROR 0xFF

// 7-Bit address options
#define VEML6030_ADDRESS 0x10

// const uint8_t defAddr = 0x48;
// const uint8_t altAddr = 0x10;

enum VEML6030_16BIT_REGISTERS {

	SETTING_REG            = 0x00,
	H_THRESH_REG,
	L_THRESH_REG,
	POWER_SAVE_REG,
	AMBIENT_LIGHT_DATA_REG,
	WHITE_LIGHT_DATA_REG,
	INTERRUPT_REG

};

enum VEML6030_16BIT_REG_MASKS {
	
	THRESH_MASK            = 0x0,
	GAIN_MASK              = 0xE7FF,
	INTEG_MASK             = 0xFC3F,
	PERS_PROT_MASK         = 0xFFCF,
	INT_EN_MASK            = 0xFFFD,
	SD_MASK                = 0xFFFE,
	POW_SAVE_EN_MASK       = 0x06, // Most of this register is reserved
	POW_SAVE_MASK          = 0x01, // Most of this register is reserved
	INT_MASK               = 0xC000
	
};

enum REGISTER_BIT_POSITIONS {

	NO_SHIFT               = 0x00,
	INT_EN_POS             = 0x01,
	PSM_POS                = 0x01,
	PERS_PROT_POS          = 0x04,
	INTEG_POS              = 0x06,
	GAIN_POS               = 0xB,
	INT_POS                = 0xE

};



bool VEML6030_begin(struct i2c_master_module* hw_module); // begin function

// REG0x00, bits [12:11]
// This function sets the gain for the Ambient Light Sensor. Possible values
// are 1/8, 1/4, 1, and 2. The highest setting should only be used if the
// sensors is behind dark glass, where as the lowest setting should be used in
// dark rooms. The datasheet suggests always leaving it at around 1/4 or 1/8.
void VEML6030_setGain(float gainVal);

// REG0x00, bits [12:11]
// This function reads the gain for the Ambient Light Sensor. Possible values
// are 1/8, 1/4, 1, and 2. The highest setting should only be used if the
// sensors is behind dark glass, where as the lowest setting should be used in
// dark rooms. The datasheet suggests always leaving it at around 1/4 or 1/8.
float VEML6030_readGain(void);

// REG0x00, bits[9:6]
// This function sets the integration time (the saturation time of light on the
// sensor) of the ambient light sensor. Higher integration time leads to better
// resolution but slower sensor refresh times.
void VEML6030_setIntegTime(uint16_t time);

// REG0x00, bits[9:6]
// This function reads the integration time (the saturation time of light on the
// sensor) of the ambient light sensor. Higher integration time leads to better
// resolution but slower sensor refresh times.
uint16_t VEML6030_readIntegTime(void);

// REG0x00, bits[5:4]
// This function sets the persistence protect number i.e. the number of
// values needing to crosh the interrupt thresholds.
void VEML6030_setProtect(uint8_t protVal);

// REG0x00, bits[5:4]
// This function reads the persistence protect number i.e. the number of
// values needing to crosh the interrupt thresholds.
uint8_t VEML6030_readProtect(void);

// REG0x00, bit[1]
// This function enables the Ambient Light Sensor's interrupt.
void VEML6030_enableInt(void);

// REG0x00, bit[1]
// This function disables the Ambient Light Sensor's interrupt.
void VEML6030_disableInt(void);

// REG0x00, bit[1]
// This function checks if the interrupt is enabled or disabled.
uint8_t VEML6030_readIntSetting(void);

// REG0x00, bit[0]
// This function powers down the Ambient Light Sensor. The light sensor will
// hold onto the last light reading which can be acessed while the sensor is
// shut down. 0.5 micro Amps are consumed while shutdown.
void VEML6030_shutDown(void);

// REG0x00, bit[0]
// This function powers up the Ambient Light Sensor. The last value that was
// read during shut down will be overwritten on the sensor's subsequent read.
// After power up, a small 4ms delay is applied to give time for the internal
// osciallator and signal processor to power up.
void VEML6030_powerOn(void);

// REG0x03, bit[0]
// This function enables the current power save mode value and puts the Ambient
// Light Sensor into power save mode.
void VEML6030_enablePowSave(void);

// REG0x03, bit[0]
// This function disables the current power save mode value and pulls the Ambient
// Light Sensor out of power save mode.
void VEML6030_disablePowSave(void);

// REG0x03, bit[0]
// This function checks to see if power save mode is enabled or disabled.
uint8_t VEML6030_readPowSavEnabled(void);

// REG0x03, bit[2:1]
// This function sets the power save mode value. It takes a value of 1-4. Each
// incrementally higher value descreases the sampling rate of the sensor and so
// increases power saving. The datasheet suggests enabling these modes when
// continually sampling the sensor.
void VEML6030_setPowSavMode(uint16_t modeVal);

// REG0x03, bit[2:1]
// This function reads the power save mode value. The function above takes a value of 1-4. Each
// incrementally higher value descreases the sampling rate of the sensor and so
// increases power saving. The datasheet suggests enabling these modes when
// continually sampling the sensor.
uint8_t VEML6030_readPowSavMode(void);

// REG0x06, bits[15:14]
// This function reads the interrupt register to see if an interrupt has been
// triggered. There are two possible interrupts: a lower limit and upper limit
// threshold, both set by the user.
uint8_t VEML6030_readInterrupt(void);

// REG0x02, bits[15:0]
// This function sets the lower limit for the Ambient Light Sensor's interrupt.
// It takes a lux value as its paramater.
void VEML6030_setIntLowThresh(uint32_t luxVal);
	
// REG0x02, bits[15:0]
// This function reads the lower limit for the Ambient Light Sensor's interrupt.
uint32_t VEML6030_readLowThresh(void);

// REG0x01, bits[15:0]
// This function sets the upper limit for the Ambient Light Sensor's interrupt.
// It takes a lux value as its paramater.
void VEML6030_setIntHighThresh(uint32_t luxVal);

// REG0x01, bits[15:0]
// This function reads the upper limit for the Ambient Light Sensor's interrupt.
uint32_t VEML6030_readHighThresh(void);

// REG[0x04], bits[15:0]
// This function gets the sensor's ambient light's lux value. The lux value is
// determined based on current gain and integration time settings. If the lux
// value exceeds 1000 then a compensation formula is applied to it.
uint32_t VEML6030_readLight(void);

// REG[0x05], bits[15:0]
// This function gets the sensor's ambient light's lux value. The lux value is
// determined based on current gain and integration time settings. If the lux
// value exceeds 1000 then a compensation formula is applied to it.
uint32_t VEML6030_readWhiteLight(void);

#endif /* VEML6030_H_ */