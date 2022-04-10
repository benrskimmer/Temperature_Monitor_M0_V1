/*
 * VEML6030.c
 *
 * Created: 11/19/2021 12:27:30 AM
 *  Author: Ben
 */ 

#include "VEML6030.h"
#include "millis.h"
#include <math.h>

// private variables
uint8_t _address;
struct i2c_master_module* _i2c_module;

// Table of lux conversion values depending on the integration time and gain.
// The arrays represent the all possible integration times and the index of the
// arrays represent the register's gain settings, which is directly analgous to
// their bit representations.
const float eightHIt[]     = {.0036, .0072, .0288, .0576};
const float fourHIt[]      = {.0072, .0144, .0576, .1152};
const float twoHIt[]       = {.0144, .0288, .1152, .2304};
const float oneHIt[]       = {.0288, .0576, .2304, .4608};
const float fiftyIt[]      = {.0576, .1152, .4608, .9216};
const float twentyFiveIt[] = {.1152, .2304, .9216, 1.8432};

// private functions 

// This function compensates for lux values over 1000. From datasheet:
// "Illumination values higher than 1000 lx show non-linearity. This
// non-linearity is the same for all sensors, so a compensation forumla..."
// etc. etc.
uint32_t _VEML6030_luxCompensation(uint32_t _luxVal);

// The lux value of the Ambient Light sensor depends on both the gain and the
// integration time settings. This function determines which conversion value
// to use by using the bit representation of the gain as an index to look up
// the conversion value in the correct integration time array. It then converts
// the value and returns it.
uint32_t _VEML6030_calculateLux(uint16_t _lightBits);

// This function does the opposite calculation then the function above. The interrupt
// threshold values given by the user are dependent on the gain and
// intergration time settings. As a result the lux value needs to be
// calculated with the current settings and this function accomplishes
// that.
uint16_t _VEML6030_calculateBits(uint32_t _luxVal);

// This function writes to a 16 bit register. Paramaters include the register's address, a mask
// for bits that are ignored, the bits to write, and the bits' starting
// position.
void _VEML6030_writeRegister(uint8_t _wReg, uint16_t _mask, uint16_t _bits, uint8_t _startPosition);

// This function reads a 16 bit register. It takes the register's
// address as its' parameter.
uint16_t _VEML6030_readRegister(uint8_t _reg);

// This function tests that a I2C device is responding to a given address
uint8_t _VEML6030_I2CTest(struct i2c_master_module* module, uint8_t addr);






// public functions

bool VEML6030_begin(struct i2c_master_module* hw_module)
{
  
  _i2c_module = hw_module;
  // Device is powered down by default. 
  VEML6030_powerOn(); 

	return (bool)_VEML6030_I2CTest(_i2c_module, VEML6030_ADDRESS);
//   _i2cPort->beginTransmission(_address);
//   uint8_t _ret = _i2cPort->endTransmission();
//   if( !_ret )
//     return true; 
//   else 
//     return false; 

}

// REG0x00, bits [12:11]
// This function sets the gain for the Ambient Light Sensor. Possible values
// are 1/8, 1/4, 1, and 2. The highest setting should only be used if the
// sensors is behind dark glass, where as the lowest setting should be used in
// dark rooms. The datasheet suggests always leaving it at around 1/4 or 1/8.
void VEML6030_setGain(float gainVal){

  uint16_t bits; 

  if (gainVal == 1.00)
    bits = 0; 
  else if (gainVal == 2.00)
    bits = 1;
  else if (gainVal == .125)
    bits = 2;
  else if (gainVal == .25)
    bits = 3; 
  else
    return; 
  
  _VEML6030_writeRegister(SETTING_REG, GAIN_MASK, bits, GAIN_POS); 

}

// REG0x00, bits [12:11]
// This function reads the gain for the Ambient Light Sensor. Possible values
// are 1/8, 1/4, 1, and 2. The highest setting should only be used if the
// sensors is behind dark glass, where as the lowest setting should be used in
// dark rooms. The datasheet suggests always leaving it at around 1/4 or 1/8.
float VEML6030_readGain(void){
 
  uint16_t regVal = _VEML6030_readRegister(SETTING_REG); // Get register
  regVal &= (~GAIN_MASK); // Invert the gain mask to _keep_ the gain
  regVal = (regVal >> GAIN_POS); // Move values to front of the line. 
   
  if (regVal == 0)
    return 1;
  else if (regVal == 1)
    return 2;
  else if (regVal == 2)
    return .125;
  else if (regVal == 3)
    return .25;
  else   
    return UNKNOWN_ERROR; 
  
}

// REG0x00, bits[9:6]
// This function sets the integration time (the saturation time of light on the
// sensor) of the ambient light sensor. Higher integration time leads to better
// resolution but slower sensor refresh times. 
void VEML6030_setIntegTime(uint16_t time){ 
 
  uint16_t bits;

  if (time == 100) // Default setting.
    bits = 0; 
  else if (time == 200)
    bits = 1; 
  else if (time == 400)
    bits = 2; 
  else if (time == 800)
    bits = 3; 
  else if (time == 50)
    bits = 8; 
  else if (time == 25)
    bits = 12; 
  else
    return;

  _VEML6030_writeRegister(SETTING_REG, INTEG_MASK, bits, INTEG_POS);  
  uint8_t regVal = VEML6030_readIntegTime();

}

// REG0x00, bits[9:6]
// This function reads the integration time (the saturation time of light on the
// sensor) of the ambient light sensor. Higher integration time leads to better
// resolution but slower sensor refresh times. 
uint16_t VEML6030_readIntegTime(void){

  uint16_t regVal = _VEML6030_readRegister(SETTING_REG); 
  regVal &= (~INTEG_MASK); 
  regVal = (regVal >> INTEG_POS); 

  if (regVal == 0)
    return 100;
  else if (regVal == 1)
    return 200;
  else if (regVal == 2)
    return 400;
  else if (regVal == 3)
    return 800;
  else if (regVal == 8)
    return 50;
  else if (regVal == 12)
    return 25;
  else   
    return UNKNOWN_ERROR; 

}

// REG0x00, bits[5:4]
// This function sets the persistence protect number. 
void VEML6030_setProtect(uint8_t protVal){

  uint16_t bits; 

  if (protVal == 1)
    bits = 0; 
  else if (protVal == 2)
    bits = 1;
  else if (protVal == 4)
    bits = 2;
  else if (protVal == 8)
    bits = 3;
  else
    return;

  _VEML6030_writeRegister(SETTING_REG, PERS_PROT_MASK, bits, PERS_PROT_POS); 

}

// REG0x00, bits[5:4]
// This function reads the persistence protect number. 
uint8_t VEML6030_readProtect(void){

  uint16_t regVal = _VEML6030_readRegister(SETTING_REG); 
  regVal &= (~PERS_PROT_MASK); 
  regVal = (regVal >> PERS_PROT_POS); 

  if (regVal == 0)
    return 1;
  else if (regVal == 1)
    return 2;
  else if (regVal == 2)
    return 4;
  else if (regVal == 3)
    return 8;
  else
    return UNKNOWN_ERROR;

}

// REG0x00, bit[1]
// This function enables the Ambient Light Sensor's interrupt. 
void VEML6030_enableInt(void){

  _VEML6030_writeRegister(SETTING_REG, INT_EN_MASK, ENABLE, INT_EN_POS); 

}

// REG0x00, bit[1]
// This function disables the Ambient Light Sensor's interrupt. 
void VEML6030_disableInt(void){

  _VEML6030_writeRegister(SETTING_REG, INT_EN_MASK, DISABLE, INT_EN_POS); 

}

// REG0x00, bit[1]
// This function checks if the interrupt is enabled or disabled. 
uint8_t VEML6030_readIntSetting(void){

  uint16_t regVal = _VEML6030_readRegister(SETTING_REG); 
  regVal &= (~INT_EN_MASK); 
  regVal = (regVal >> INT_EN_POS); 
  return regVal;

}

// REG0x00, bit[0]
// This function powers down the Ambient Light Sensor. The light sensor will
// hold onto the last light reading which can be acessed while the sensor is 
// shut down. 0.5 micro Amps are consumed while shutdown. 
void VEML6030_shutDown(void){

  _VEML6030_writeRegister(SETTING_REG, SD_MASK, SHUTDOWN , NO_SHIFT);

}

// REG0x00, bit[0]
// This function powers up the Ambient Light Sensor. The last value that was
// read during shut down will be overwritten on the sensor's subsequent read.
// After power up, a small 4ms delay is applied to give time for the internal
// osciallator and signal processor to power up.   
void VEML6030_powerOn(void){

  _VEML6030_writeRegister(SETTING_REG, SD_MASK, POWER, NO_SHIFT);
  delay(4);

}

// REG0x03, bit[0]
// This function enables the current power save mode value and puts the Ambient
// Light Sensor into power save mode. 
void VEML6030_enablePowSave(void){
    
  _VEML6030_writeRegister(POWER_SAVE_REG, POW_SAVE_EN_MASK, ENABLE, NO_SHIFT);  

}

// REG0x03, bit[0]
// This function disables the current power save mode value and pulls the Ambient
// Light Sensor out of power save mode. 
void VEML6030_disablePowSave(void){

  _VEML6030_writeRegister(POWER_SAVE_REG, POW_SAVE_EN_MASK, DISABLE, NO_SHIFT);  

}

// REG0x03, bit[0]
// This function checks to see if power save mode is enabled or disabled. 
uint8_t VEML6030_readPowSavEnabled(void){

  uint16_t regVal = _VEML6030_readRegister(POWER_SAVE_REG); 
  regVal &= (~POW_SAVE_EN_MASK); 
  return regVal;

}

// REG0x03, bit[2:1]
// This function sets the power save mode value. It takes a value of 1-4. Each
// incrementally higher value descreases the sampling rate of the sensor and so
// increases power saving. The datasheet suggests enabling these modes when
// continually sampling the sensor. 
void VEML6030_setPowSavMode(uint16_t modeVal){

  uint16_t bits; 

  if (modeVal == 1)
    bits = 0;
  else if (modeVal == 2)
    bits = 1;
  else if (modeVal == 3)
    bits = 2;
  else if (modeVal == 4)
    bits = 3;
  else 
    return; 

  _VEML6030_writeRegister(POWER_SAVE_REG, POW_SAVE_MASK, bits, PSM_POS);  

}

// REG0x03, bit[2:1]
// This function reads the power save mode value. The function above takes a value of 1-4. Each
// incrementally higher value descreases the sampling rate of the sensor and so
// increases power saving. The datasheet suggests enabling these modes when
// continually sampling the sensor. 
uint8_t VEML6030_readPowSavMode(void){

  uint16_t regVal = _VEML6030_readRegister(POWER_SAVE_REG); 
  regVal &= (~POW_SAVE_MASK); 
  regVal = (regVal >> PSM_POS); 
  
  if (regVal == 0)
    return 1; 
  else if (regVal == 1)
    return 2;
  else if (regVal == 2)
    return 3;
  else if (regVal == 3)
    return 4;
  else 
    return UNKNOWN_ERROR;

}

// REG0x06, bits[15:14]
// This function reads the interrupt register to see if an interrupt has been
// triggered. There are two possible interrupts: a lower limit and upper limit 
// threshold, both set by the user.  
uint8_t VEML6030_readInterrupt(void){

  uint16_t regVal = _VEML6030_readRegister(INTERRUPT_REG); 
  regVal &= INT_MASK; 
  regVal = (regVal >> INT_POS); 

  if (regVal == 0)
    return NO_INT;
  else if (regVal == 1)
    return INT_HIGH;
  else if (regVal == 2)
    return INT_LOW;
  else
    return UNKNOWN_ERROR;

}

// REG0x02, bits[15:0]
// This function sets the lower limit for the Ambient Light Sensor's interrupt. 
// It takes a lux value as its paramater.
void VEML6030_setIntLowThresh(uint32_t luxVal){

  if (luxVal < 0 || luxVal > 120000)
    return;
  
  uint16_t luxBits = _VEML6030_calculateBits(luxVal); 
  _VEML6030_writeRegister(L_THRESH_REG, THRESH_MASK, luxBits, NO_SHIFT);

}

// REG0x02, bits[15:0]
// This function reads the lower limit for the Ambient Light Sensor's interrupt. 
uint32_t VEML6030_readLowThresh(){

  uint16_t threshVal = _VEML6030_readRegister(L_THRESH_REG);
  uint32_t threshLux = _VEML6030_calculateLux(threshVal); 
  return threshLux; 

}

// REG0x01, bits[15:0]
// This function sets the upper limit for the Ambient Light Sensor's interrupt. 
// It takes a lux value as its paramater.
void VEML6030_setIntHighThresh(uint32_t luxVal){

  if (luxVal < 0 || luxVal > 120000)
    return;

  uint16_t luxBits = _VEML6030_calculateBits(luxVal); 
  _VEML6030_writeRegister(H_THRESH_REG, THRESH_MASK, luxBits, NO_SHIFT);

}

// REG0x01, bits[15:0]
// This function reads the upper limit for the Ambient Light Sensor's interrupt. 
uint32_t VEML6030_readHighThresh(void){

  uint16_t threshVal = _VEML6030_readRegister(H_THRESH_REG);
  uint32_t threshLux = _VEML6030_calculateLux(threshVal); 
  return threshLux; 

}

// REG[0x04], bits[15:0]
// This function gets the sensor's ambient light's lux value. The lux value is
// determined based on current gain and integration time settings. If the lux
// value exceeds 1000 then a compensation formula is applied to it. 
uint32_t VEML6030_readLight(void){

  uint16_t lightBits =  _VEML6030_readRegister(AMBIENT_LIGHT_DATA_REG); 
  uint32_t luxVal = _VEML6030_calculateLux(lightBits); 

  if (luxVal > 1000) {
    uint32_t compLux = _VEML6030_luxCompensation(luxVal); 
    return compLux; 
  }
  else
    return luxVal;

}

// REG[0x05], bits[15:0]
// This function gets the sensor's ambient light's lux value. The lux value is
// determined based on current gain and integration time settings. If the lux
// value exceeds 1000 then a compensation formula is applied to it. 
uint32_t VEML6030_readWhiteLight(void){

  uint16_t lightBits = _VEML6030_readRegister(WHITE_LIGHT_DATA_REG); 
  uint32_t luxVal = _VEML6030_calculateLux(lightBits); 

  if (luxVal > 1000) {
    uint32_t compLux = _VEML6030_luxCompensation(luxVal); 
    return compLux; 
  }
  else
    return luxVal;

}

// This function compensates for lux values over 1000. From datasheet:
// "Illumination values higher than 1000 lx show non-linearity. This
// non-linearity is the same for all sensors, so a compensation forumla..."
// etc. etc. 
uint32_t _VEML6030_luxCompensation(uint32_t _luxVal){ 

  // Polynomial is pulled from pg 10 of the datasheet. 
  uint32_t _compLux = (.00000000000060135 * (pow(_luxVal, 4))) - 
                      (.0000000093924 * (pow(_luxVal, 3))) + 
                      (.000081488 * (pow(_luxVal,2))) + 
                      (1.0023 * _luxVal);
  return _compLux;

}

// The lux value of the Ambient Light sensor depends on both the gain and the
// integration time settings. This function determines which conversion value
// to use by using the bit representation of the gain as an index to look up
// the conversion value in the correct integration time array. It then converts 
// the value and returns it.  
uint32_t _VEML6030_calculateLux(uint16_t _lightBits){

  float _luxConv; 
  uint8_t _convPos;  

  float _gain = VEML6030_readGain(); 
  uint16_t _integTime = VEML6030_readIntegTime();

  // Here the gain is checked to get the position of the conversion value
  // within the integration time arrays. These values also represent the bit
  // values for setting the gain. 
  if (_gain == 2.00) 
    _convPos = 0;
  else if (_gain == 1.00)
    _convPos = 1; 
  else if (_gain == .25)
    _convPos = 2; 
  else if (_gain == .125)
    _convPos = 3; 
  else
    return UNKNOWN_ERROR;

  // Here we check the integration time which determines which array we probe
  // at the position determined above.
  if(_integTime == 800)
    _luxConv = eightHIt[_convPos]; 
  else if(_integTime == 400)
    _luxConv = fourHIt[_convPos];
  else if(_integTime == 200)
    _luxConv = twoHIt[_convPos];
  else if(_integTime == 100)
    _luxConv = oneHIt[_convPos];
  else if(_integTime == 50)
    _luxConv = fiftyIt[_convPos];
  else if(_integTime == 25)
    _luxConv = twentyFiveIt[_convPos];
  else
    return UNKNOWN_ERROR; 

  // Multiply the value from the 16 bit register to the conversion value and return
  // it. 
  uint32_t _calculatedLux = (_luxConv * _lightBits);
  return _calculatedLux;

}


// This function does the opposite calculation then the function above. The interrupt
// threshold values given by the user are dependent on the gain and
// intergration time settings. As a result the lux value needs to be
// calculated with the current settings and this function accomplishes
// that.  
uint16_t _VEML6030_calculateBits(uint32_t _luxVal){

  float _luxConv; 
  uint8_t _convPos;  

  float _gain = VEML6030_readGain();
  float _integTime = VEML6030_readIntegTime();
  // Here the gain is checked to get the position of the conversion value
  // within the integration time arrays. These values also represent the bit
  // values for setting the gain. 
  if (_gain == 2.00) 
    _convPos = 0;
  else if (_gain == 1.00)
    _convPos = 1; 
  else if (_gain == .25)
    _convPos = 2; 
  else if (_gain == .125)
    _convPos = 3; 
  else
    return UNKNOWN_ERROR;

  // Here we check the integration time which determines which array we probe
  // at the position determined above.
  if(_integTime == 800)
    _luxConv = eightHIt[_convPos]; 
  else if(_integTime == 400)
    _luxConv = fourHIt[_convPos];
  else if(_integTime == 200)
    _luxConv = twoHIt[_convPos];
  else if(_integTime == 100)
    _luxConv = oneHIt[_convPos];
  else if(_integTime == 50)
    _luxConv = fiftyIt[_convPos];
  else if(_integTime == 25)
    _luxConv = twentyFiveIt[_convPos];
  else
    return UNKNOWN_ERROR; 

  // Divide the value of lux bythe conversion value and return
  // it. 
  uint16_t _calculatedBits = (_luxVal/_luxConv);
  return _calculatedBits;

}

// This function writes to a 16 bit register. Paramaters include the register's address, a mask 
// for bits that are ignored, the bits to write, and the bits' starting
// position.
void _VEML6030_writeRegister(uint8_t _wReg, uint16_t _mask, uint16_t _bits, uint8_t _startPosition)
{
  uint8_t buffer[3] = {0};
	  
  struct i2c_master_packet packet = {
	  .address = VEML6030_ADDRESS,
	  .data = buffer,
	  .data_length = 3,
	  .ten_bit_address = false,
	  .high_speed      = false,
	  .hs_master_code  = 0x0,
  };
  
  uint16_t _i2cWrite; 

  _i2cWrite = _VEML6030_readRegister(_wReg); // Get the current value of the register
  _i2cWrite &= _mask; // Mask the position we want to write to.
  _i2cWrite |= (_bits << _startPosition);  // Place the given bits to the variable
  
  buffer[0] = _wReg;
  buffer[1] = (uint8_t)_i2cWrite;
  buffer[2] = (uint8_t)(_i2cWrite >> 8);
  
  i2c_master_write_packet_wait(_i2c_module, &packet);
  
//   _i2cPort->beginTransmission(_address); // Start communication.
//   _i2cPort->write(_wReg); // at register....
//   _i2cPort->write(_i2cWrite); // Write LSB to register...
//   _i2cPort->write(_i2cWrite >> 8); // Write MSB to register...
//   _i2cPort->endTransmission(); // End communcation.
}

// This function reads a 16 bit register. It takes the register's
// address as its' parameter.
uint16_t _VEML6030_readRegister(uint8_t _reg)
{
  uint8_t buffer[2] = {_reg,0};
  uint16_t _regValue;
  
  struct i2c_master_packet packet = {
	  .address = VEML6030_ADDRESS,
	  .data = buffer,
	  .data_length = 1,
	  .ten_bit_address = false,
	  .high_speed      = false,
	  .hs_master_code  = 0x0,
  };
 
  
  //write to the register
  if(i2c_master_write_packet_wait_no_stop(_i2c_module, &packet) != STATUS_OK) return 0;
  
  packet.data_length = 2; // prepare to read two bytes back
  buffer[0] = 0;
  
  //read two bytes back
  if(i2c_master_read_packet_wait(_i2c_module, &packet) != STATUS_OK) return 0;
  
  _regValue = buffer[0]; // LSB
  _regValue |= (((uint16_t)(buffer[1]) << 8) & 0xFF00); //MSB
  
  return _regValue;

}

uint8_t _VEML6030_I2CTest(struct i2c_master_module* module, uint8_t addr)
{
	uint8_t response = 0;
	uint8_t temp[1] = {0xFF};
	
	struct i2c_master_packet packet = {
		.address = addr,
		.data = temp,
		.data_length = 0,
		.ten_bit_address = false,
		.high_speed      = false,
		.hs_master_code  = 0x0,
	};

	// We use the return value of
	// the Write.endTransmisstion to see if
	// a device did acknowledge to the address.
	// 	Wire.beginTransmission(addr);
	// 	response = !Wire.endTransmission();
	response = (i2c_master_write_packet_wait(module, &packet) == STATUS_OK);

	return response;

} /* I2CTest() */