/*
 * Si7021.c
 *
 * Created: 11/8/2021 12:19:23 AM
 *  Author: Ben
 */ 

#include <c_types.h>
#include <math.h>
#include "Si7021.h"
#include "millis.h"

// private variables
si_sensorType _model;
uint8_t _revision;
static struct i2c_master_module* _i2c_module = NULL;          ///< Pointer to I2C bus interface
const static int _TRANSACTION_TIMEOUT = 100; // Wire NAK/Busy timeout in ms

//private functions
static void _readRevision();
static uint8_t _readRegister8(uint8_t reg);
static void _writeRegister8(uint8_t reg, uint8_t value);
BOOL _Si7021_read(unsigned char *data, int len);
BOOL _Si7021_write(unsigned char *data, int len);


/*!
 *  @brief  Instantiates a new Adafruit_Si7021 class
 *  @param  *theWire
 *          optional wire object
 */
// Si7021_Adafruit_Si7021(TwoWire *theWire) {
//   i2c_dev = new Adafruit_I2CDevice(SI7021_DEFAULT_ADDRESS, theWire);
// 
//   sernum_a = sernum_b = 0;
//   _model = SI_UNKNOWN;
//   _revision = 0;
// }

/*!
 *  @brief  Sets up the HW by reseting It, reading serial number and reading
 * revision.
 *  @return Returns true if set up is successful.
 */
BOOL Si7021_begin(struct i2c_master_module* hw_module) {
  _i2c_module = hw_module;

  Si7021_reset();
  if (_readRegister8(SI7021_READRHT_REG_CMD) != 0x3A)
    return false;

  Si7021_readSerialNumber();
  _readRevision();

  return true;
}

/*!
 *  @brief  Reads the humidity value from Si7021 (No Master hold)
 *  @return Returns humidity as float value or NAN when there is error timeout
 */
float Si7021_readHumidity(void) {
  uint8_t buffer[3] = {SI7021_MEASRH_NOHOLD_CMD, 0, 0};

  if (!_Si7021_write(buffer, 1))
    return NAN;

  delay(20); // account for conversion time for reading humidity

  uint32_t start = millis(); // start timeout
  while (millis() - start < _TRANSACTION_TIMEOUT) {
    if (_Si7021_read(buffer, 3)) {
      uint16_t hum = buffer[0] << 8 | buffer[1];
      // uint8_t chxsum = buffer[2];

      float humidity = hum;
      humidity *= 125;
      humidity /= 65536;
      humidity -= 6;

      return humidity > 100.0 ? 100.0 : humidity;
    }
    delay(6); // 1/2 typical sample processing time
  }
  return NAN; // Error timeout
}

/*!
 *  @brief  Reads the temperature value from Si7021 (No Master hold)
 *  @return Returns temperature as float value or NAN when there is error
 * timeout
 */
float Si7021_readTemperature(void) {
  uint8_t buffer[3] = {SI7021_MEASTEMP_NOHOLD_CMD, 0, 0};

  if (!_Si7021_write(buffer, 1))
    return NAN;

  delay(20); // account for conversion time for reading temperature

  uint32_t start = millis(); // start timeout
  while (millis() - start < _TRANSACTION_TIMEOUT) {
    if (_Si7021_read(buffer, 3)) {
      uint16_t temp = buffer[0] << 8 | buffer[1];
      // uint8_t chxsum = buffer[2];

      float temperature = temp;
      temperature *= 175.72;
      temperature /= 65536;
      temperature -= 46.85;
      return temperature;
    }
    delay(6); // 1/2 typical sample processing time
  }

  return NAN; // Error timeout
}

/*!
 *  @brief  Sends the reset command to Si7021.
 */
void Si7021_reset(void) {
  uint8_t buffer[1] = {SI7021_RESET_CMD};
  _Si7021_write(buffer, 1);

  delay(50);
}

uint8_t Si7021_getRevision() { return _revision; };

void _readRevision(void) {
	uint8_t buffer[2] = {(uint8_t)(SI7021_FIRMVERS_CMD >> 8),
                        (uint8_t)(SI7021_FIRMVERS_CMD & 0xFF)};

//	_Si7021_write_then_read(buffer, 2, buffer, 1);
  
	_Si7021_write(buffer, 2);
	_Si7021_read(buffer, 1);

	switch (buffer[0]) {
	case SI7021_REV_1:
	_revision = 1;
	break;
	case SI7021_REV_2:
	_revision = 2;
	break;
	default:
	_revision = 0;
	}
}

/*!
 *  @brief  Reads serial number and stores It in sernum_a and sernum_b variable
 */
void Si7021_readSerialNumber(void) {
  uint8_t buffer[8];

  //
  // SNA
  //
  buffer[0] = (uint8_t)(SI7021_ID1_CMD >> 8);
  buffer[1] = (uint8_t)(SI7021_ID1_CMD & 0xFF);
//   _Si7021_write_then_read(buffer, 2, buffer, 8);
  _Si7021_write(buffer, 2);
  _Si7021_read(buffer, 8);
  sernum_a = (uint32_t)(buffer[0]) << 24 | (uint32_t)(buffer[2]) << 16 |
             (uint32_t)(buffer[4]) << 8 | (uint32_t)(buffer[6]);

  //
  // SNB
  //
  buffer[0] = (uint8_t)(SI7021_ID2_CMD >> 8);
  buffer[1] = (uint8_t)(SI7021_ID2_CMD & 0xFF);
//  _Si7021_write_then_read(buffer, 2, buffer, 6);
  _Si7021_write(buffer, 2);
  _Si7021_read(buffer, 6);
  sernum_a = (uint32_t)(buffer[0]) << 24 | (uint32_t)(buffer[1]) << 16 |
             (uint32_t)(buffer[4]) << 8 | (uint32_t)(buffer[5]);

  // Use SNB3 for device model version
  switch (buffer[0]) {
  case 0:
  case 0xff:
    _model = SI_Engineering_Samples;
    break;
  case 0x0D:
    _model = SI_7013;
    break;
  case 0x14:
    _model = SI_7020;
    break;
  case 0x15:
    _model = SI_7021;
    break;
  default:
    _model = SI_UNKNOWN;
  }
}

/*!
 *  @brief  Returns sensor model established during init
 *  @return model value
 */
si_sensorType Si7021_getModel() { return _model; }

/*!
 *  @brief  Enable/Disable sensor heater
 */
void Si7021_heater(bool h) {
  uint8_t regValue = _readRegister8(SI7021_READRHT_REG_CMD);

  if (h) {
    regValue |= (1 << (SI7021_REG_HTRE_BIT));
  } else {
    regValue &= ~(1 << (SI7021_REG_HTRE_BIT));
  }
  _writeRegister8(SI7021_WRITERHT_REG_CMD, regValue);
}

/*!
 *  @brief  Return sensor heater state
 *  @return heater state (TRUE = enabled, FALSE = disabled)
 */
bool Si7021_isHeaterEnabled(void) {
  uint8_t regValue = _readRegister8(SI7021_READRHT_REG_CMD);
  return (bool)(regValue & SI7021_REG_HTRE_BIT);
}

/*!
 *  @brief  Set the sensor heater heat level
 */
void Si7021_setHeatLevel(uint8_t level) {
  _writeRegister8(SI7021_WRITEHEATER_REG_CMD, level);
}

/*******************************************************************/

void _writeRegister8(uint8_t reg, uint8_t value) {
   uint8_t buffer[2] = {reg, value};
//   i2c_dev->write(buffer, 2);

	struct i2c_master_packet packet = {
		.address = SI7021_DEFAULT_ADDRESS,
		.data = buffer,
		.data_length = 2,
		.ten_bit_address = false,
		.high_speed      = false,
		.hs_master_code  = 0x0,
	};

	i2c_master_write_packet_wait(_i2c_module, &packet);

  // Serial.print("Wrote $"); Serial.print(reg, HEX); Serial.print(": 0x");
  // Serial.println(value, HEX);
}

uint8_t _readRegister8(uint8_t reg) {
	uint8_t buffer[1] = {reg};
//  i2c_dev->write_then_read(buffer, 1, buffer, 1);
	
	struct i2c_master_packet packet = {
		.address = SI7021_DEFAULT_ADDRESS,
		.data = buffer,
		.data_length = 1,
		.ten_bit_address = false,
		.high_speed      = false,
		.hs_master_code  = 0x0,
	};
	
	if(i2c_master_write_packet_wait(_i2c_module, &packet) != STATUS_OK) return 0;

	packet.data[0] = 0;
	if(i2c_master_read_packet_wait(_i2c_module, &packet) != STATUS_OK) return 0;
	
	return packet.data[0];
}

BOOL _Si7021_write(unsigned char *data, int len) {
//	struct i2c_master_packet packet;
// 	packet.address = SI7021_DEFAULT_ADDRESS;
// 	packet.data = data;
// 	packet.data_length = len;
	
	struct i2c_master_packet packet = {
		.address = SI7021_DEFAULT_ADDRESS,
		.data = data,
		.data_length = len,
		.ten_bit_address = false,
		.high_speed      = false,
		.hs_master_code  = 0x0,
	};

	if(i2c_master_write_packet_wait(_i2c_module, &packet) != STATUS_OK) return 0;
	
	return 1;
}

BOOL _Si7021_read(unsigned char *data, int len) {

	struct i2c_master_packet packet = {
		.address = SI7021_DEFAULT_ADDRESS,
		.data = data,
		.data_length = len,
		.ten_bit_address = false,
		.high_speed      = false,
		.hs_master_code  = 0x0,
	};

	if(i2c_master_read_packet_wait(_i2c_module, &packet) != STATUS_OK) return 0;
	
	return 1;
}