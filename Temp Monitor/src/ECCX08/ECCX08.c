/*
  This file is part of the ArduinoECCX08 library.
  Copyright (c) 2018 Arduino SA. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

//#include <Arduino.h>

#include "ECCX08.h"
#include "millis.h"
#include <string.h>
#include "i2c_master.h"

uint32_t wakeupFrequency = 100000u;  // 100 kHz
uint32_t normalFrequency = 1000000u; // 1 MHz



static struct i2c_master_module* _i2c_module = NULL;
//uint8_t EECX08_ADDRESS;



// private functions
int wakeup(void);
int sleep(void);
int idle(void);

long version(void);
int challenge(uint8_t* message);
int verify(uint8_t* signature, uint8_t* pubkey);
static int sign(int slot, uint8_t* signature);

int read(int zone, int address, uint8_t* buffer, int length);
int write(int zone, int address, uint8_t* buffer, int length);
int lock(int zone);

int addressForSlotOffset(int slot, int offset);

int sendCommand(uint8_t opcode, uint8_t param1, uint16_t param2, uint8_t* data, size_t dataLength); // default data to NULL, dataLength to 0
int receiveResponse(void* response, size_t length);
uint16_t crc16(uint8_t* data, size_t length);


void ECCX08_init(struct i2c_master_module* module)
{
	_i2c_module = module;
}

BOOL ECCX08_begin(void){
  
  wakeup();
  idle();
  
  volatile long ver = version() & 0x0F00000; // reset to non-volatile, switched for debugging

  if (ver != 0x0500000 && ver != 0x0600000) {
	  return 0;
  }

  return 1;
}



void ECCX08_end(void)
{
  // First wake up the device otherwise the chip won't react to a sleep command
  wakeup();
  sleep();
}

int ECCX08_serialNumber(uint8_t* sn)
{
  if (!read(0, 0, &sn[0], 4)) {
    return 0;
  }

  if (!read(0, 2, &sn[4], 4)) {
    return 0;
  }

  if (!read(0, 3, &sn[8], 4)) {
    return 0;
  }

  return 1;
}

// String ECCX08_getSerialNumber(void)
// {
//   String result = (char*)NULL;
//   uint8_t sn[12];
// 
//   if (!ECCX08_serialNumber(sn)) {
//     return result;
//   }
// 
//   result.reserve(18);
// 
//   for (int i = 0; i < 9; i++) {
//     uint8_t b = sn[i];
// 
//     if (b < 16) {
//       result += "0";
//     }
//     result += String(b, HEX);
//   }
// 
//   result.toUpperCase();
// 
//   return result;
// }

long ECCX08_random(long max)
{
  return ECCX08_random_range(0, max);
}

long ECCX08_random_range(long min, long max)
{
  if (min >= max)
  {
    return min;
  }

  long diff = max - min;

  long r;
  ECCX08_random_array((uint8_t*)&r, sizeof(r));

  if (r < 0) {
    r = -r;
  }

  r = (r % diff);

  return (r + min);
}

int ECCX08_random_array(uint8_t* data, size_t length)
{
  if (!wakeup()) {
    return 0;
  }

  while (length) {
    if (!sendCommand(0x1b, 0x00, 0x0000, NULL, 0)) {
      return 0;
    }

    delay(23);

    uint8_t response[32];

    if (!receiveResponse(response, sizeof(response))) {
      return 0;
    }

    int copyLength = min(32, (int)length);
    memcpy(data, response, copyLength);

    length -= copyLength;
    data += copyLength;
  }

  delay(1);

  idle();

  return 1;
}

int ECCX08_generatePrivateKey(int slot, uint8_t* publicKey)
{
  if (!wakeup()) {
    return 0;
  }

  if (!sendCommand(0x40, 0x04, slot, NULL, 0)) {
    return 0;
  }

  delay(115);

  if (!receiveResponse(publicKey, 64)) {
    return 0;
  }

  delay(1);

  idle();

  return 1;
}

int ECCX08_generatePublicKey(int slot, uint8_t* publicKey)
{
  if (!wakeup()) {
    return 0;
  }

  if (!sendCommand(0x40, 0x00, slot, NULL, 0)) {
    return 0;
  }

  delay(115);

  if (!receiveResponse(publicKey, 64)) {
    return 0;
  }

  delay(1);

  idle();

  return 1;
}

int ECCX08_ecdsaVerify(uint8_t* message, uint8_t* signature, uint8_t* pubkey)
{
  if (!challenge(message)) {
    return 0;
  }

  if (!verify(signature, pubkey)) {
    return 0;
  }

  return 1;
}

int ECCX08_ecSign(int slot, uint8_t* message, uint8_t* signature)
{
  uint8_t rand[32];

  if (!ECCX08_random_array(rand, sizeof(rand))) {
    return 0;
  }

  if (!challenge(message)) {
    return 0;
  }

  if (!sign(slot, signature)) {
    return 0;
  }

  return 1;
}

int ECCX08_beginSHA256(void)
{
  uint8_t status;

  if (!wakeup()) {
    return 0;
  }

  if (!sendCommand(0x47, 0x00, 0x0000, NULL, 0)) {
    return 0;
  }

  delay(9);

  if (!receiveResponse(&status, sizeof(status))) {
    return 0;
  }

  delay(1);
  idle();

  if (status != 0) {
    return 0;
  }

  return 1;
}

int ECCX08_updateSHA256(uint8_t* data)
{
  uint8_t status;

  if (!wakeup()) {
    return 0;
  }

  if (!sendCommand(0x47, 0x01, 64, data, 64)) {
    return 0;
  }

  delay(9);

  if (!receiveResponse(&status, sizeof(status))) {
    return 0;
  }

  delay(1);
  idle();

  if (status != 0) {
    return 0;
  }

  return 1;
}

int ECCX08_endSHA256(uint8_t* result)
{
  return ECCX08_endSHA256Data(NULL, 0, result);
}

int ECCX08_endSHA256Data(uint8_t* data, int length, uint8_t* result)
{
  if (!wakeup()) {
    return 0;
  }

  if (!sendCommand(0x47, 0x02, length, data, length)) {
    return 0;
  }

  delay(9);

  if (!receiveResponse(result, 32)) {
    return 0;
  }

  delay(1);
  idle();

  return 1;
}

int ECCX08_readSlot(int slot, uint8_t* data, int length)
{
  if (slot < 0 || slot > 15) {
    return -1;
  }

  if (length % 4 != 0) {
    return 0;
  }

  int chunkSize = 32;

  for (int i = 0; i < length; i += chunkSize) {
    if ((length - i) < 32) {
      chunkSize = 4;
    }

    if (!read(2, addressForSlotOffset(slot, i), &data[i], chunkSize)) {
      return 0;
    }
  }

  return 1;
}

int ECCX08_writeSlot(int slot, uint8_t* data, int length)
{
  if (slot < 0 || slot > 15) {
    return -1;
  }

  if (length % 4 != 0) {
    return 0;
  }

  int chunkSize = 32;

  for (int i = 0; i < length; i += chunkSize) {
    if ((length - i) < 32) {
      chunkSize = 4;
    }

    if (!write(2, addressForSlotOffset(slot, i), &data[i], chunkSize)) {
      return 0;
    }
  }

  return 1;
}

int ECCX08_locked(void)
{
  uint8_t config[4];

  if (!read(0, 0x15, config, sizeof(config))) {
    return 0;
  }

  if (config[2] == 0x00 && config[3] == 0x00) {
    return 1; // locked
  }

  return 0;
}

int ECCX08_writeConfiguration(uint8_t* data)
{
  // skip first 16 bytes, they are not writable
  for (int i = 16; i < 128; i += 4) {
    if (i == 84) {
      // not writable
      continue;
    }

    if (!write(0, i / 4, &data[i], 4)) {
      return 0;
    }
  }

  return 1;
}

int ECCX08_readConfiguration(uint8_t* data)
{
  for (int i = 0; i < 128; i += 32) {
    if (!read(0, i / 4, &data[i], 32)) {
      return 0;
    }
  }

  return 1;
}

int ECCX08_lock(void)
{
  // lock config
  if (!lock(0)) {
    return 0;
  }

  // lock data and OTP
  if (!lock(1)) {
    return 0;
  }

  return 1;
}

int wakeup(void)
{
//   _i2c_module->setClock(wakeupFrequency);
//   _i2c_module->beginTransmission(0x00);
//   _i2c_module->endTransmission();

	uint8_t data = 0xFF;

	struct i2c_master_packet packet = {
		.address = 0x00,
		.data = &data,
		.data_length = 0,
		.ten_bit_address = false,
		.high_speed      = false,
		.hs_master_code  = 0x0,
	};

	if(i2c_master_write_packet_wait(_i2c_module, &packet) != STATUS_OK) return 0;

//	delayMicroseconds(1500);
	delay(2);

	uint8_t response;

	if (!receiveResponse(&response, sizeof(response)) || response != 0x11) {
		return 0;
	}

//	_i2c_module->setClock(normalFrequency); // not doing clock switching for now

	return 1;
}

int sleep(void)
{
//   _i2c_module->beginTransmission(EECX08_ADDRESS);
//   _i2c_module->write(0x01);
// 
//   if (_i2c_module->endTransmission() != 0) {
//     return 0;
//   }

	uint8_t data = 0x01;
	
	struct i2c_master_packet packet = {
		.address = EECX08_ADDRESS,
		.data = &data,
		.data_length = 1,
		.ten_bit_address = false,
		.high_speed      = false,
		.hs_master_code  = 0x0,
	};

	if(i2c_master_write_packet_wait(_i2c_module, &packet) != STATUS_OK) return 0;

	delay(1);

	return 1;
}

int idle(void)
{
//   _i2c_module->beginTransmission(EECX08_ADDRESS);
//   _i2c_module->write(0x02);
// 
//   if (_i2c_module->endTransmission() != 0) {
//     return 0;
//   }
  
  uint8_t data = 0x02;
  
  struct i2c_master_packet packet = {
	  .address = EECX08_ADDRESS,
	  .data = &data,
	  .data_length = 1,
	  .ten_bit_address = false,
	  .high_speed      = false,
	  .hs_master_code  = 0x0,
  };

  if(i2c_master_write_packet_wait(_i2c_module, &packet) != STATUS_OK) return 0;

  delay(1);

  return 1;
}

long version(void)
{
  uint32_t ic_version = 0;

  if (!wakeup()) {
    return 0;
  }

  if (!sendCommand(0x30, 0x00, 0x0000, NULL, 0)) {
    return 0;
  }

  delay(2);

  if (!receiveResponse(&ic_version, sizeof(ic_version))) {
    return 0;
  }

  delay(1);
  idle();

  return ic_version;
}

int challenge(uint8_t* message)
{
  uint8_t status;

  if (!wakeup()) {
    return 0;
  }

  // Nounce, pass through
  if (!sendCommand(0x16, 0x03, 0x0000, message, 32)) {
    return 0;
  }

  delay(29);

  if (!receiveResponse(&status, sizeof(status))) {
    return 0;
  }

  delay(1);
  idle();

  if (status != 0) {
    return 0;
  }

  return 1;
}

int verify(uint8_t* signature, uint8_t* pubkey)
{
  uint8_t status;

  if (!wakeup()) {
    return 0;
  }

  uint8_t data[128];
  memcpy(&data[0], signature, 64);
  memcpy(&data[64], pubkey, 64);

  // Verify, external, P256
  if (!sendCommand(0x45, 0x02, 0x0004, data, sizeof(data))) {
    return 0;
  }

  delay(72);

  if (!receiveResponse(&status, sizeof(status))) {
    return 0;
  }

  delay(1);
  idle();

  if (status != 0) {
    return 0;
  }

  return 1;
}

int sign(int slot, uint8_t* signature)
{
  if (!wakeup()) {
    return 0;
  }

  if (!sendCommand(0x41, 0x80, slot, NULL, 0)) {
    return 0;
  }

  delay(70);

  if (!receiveResponse(signature, 64)) {
    return 0;
  }

  delay(1);
  idle();

  return 1;
}

int read(int zone, int address, uint8_t* buffer, int length)
{
  if (!wakeup()) {
    return 0;
  }

  if (length != 4 && length != 32) {
    return 0;
  }

  if (length == 32) {
    zone |= 0x80;
  }

  if (!sendCommand(0x02, zone, address, NULL, 0)) {
    return 0;
  }

  delay(5);

  if (!receiveResponse(buffer, length)) {
    return 0;
  }

  delay(1);
  idle();

  return length;
}

int write(int zone, int address, uint8_t* buffer, int length)
{
  uint8_t status;

  if (!wakeup()) {
    return 0;
  }

  if (length != 4 && length != 32) {
    return 0;
  }

  if (length == 32) {
    zone |= 0x80;
  }

  if (!sendCommand(0x12, zone, address, buffer, length)) {
    return 0;
  }

  delay(26);

  if (!receiveResponse(&status, sizeof(status))) {
    return 0;
  }

  delay(1);
  idle();

  if (status != 0) {
    return 0;
  }

  return 1;
}

int lock(int zone)
{
  uint8_t status;

  if (!wakeup()) {
    return 0;
  }

  if (!sendCommand(0x17, 0x80 | zone, 0x0000, NULL, 0)) {
    return 0;
  }

  delay(32);

  if (!receiveResponse(&status, sizeof(status))) {
    return 0;
  }

  delay(1);
  idle();

  if (status != 0) {
    return 0;
  }

  return 1;
}

int addressForSlotOffset(int slot, int offset)
{
  int block = offset / 32;
  offset = (offset % 32) / 4;  

  return (slot << 3) | (block << 8) | (offset);
}

int sendCommand(uint8_t opcode, uint8_t param1, uint16_t param2, uint8_t* data, size_t dataLength)
{
  int commandLength = 8 + dataLength; // 1 for type, 1 for length, 1 for opcode, 1 for param1, 2 for param2, 2 for crc
  uint8_t command[commandLength]; 
  
  command[0] = 0x03;
  command[1] = sizeof(command) - 1;
  command[2] = opcode;
  command[3] = param1;
  memcpy(&command[4], &param2, sizeof(param2));
  memcpy(&command[6], data, dataLength);

  uint16_t crc = crc16(&command[1], 8 - 3 + dataLength);
  memcpy(&command[6 + dataLength], &crc, sizeof(crc));
  
  struct i2c_master_packet packet = {
	  .address = EECX08_ADDRESS,
	  .data = command,
	  .data_length = commandLength,
	  .ten_bit_address = false,
	  .high_speed      = false,
	  .hs_master_code  = 0x0,
  };

  if(i2c_master_write_packet_wait(_i2c_module, &packet) != STATUS_OK) return 0;

//   _i2c_module->beginTransmission(EECX08_ADDRESS);
//   _i2c_module->write(command, commandLength);
//   if (_i2c_module->endTransmission() != 0) {
//     return 0;
//   }

  return 1;
}

int receiveResponse(void* response, size_t length)
{
//	int retries = 20;
	size_t responseSize = length + 3; // 1 for length header, 2 for CRC
	uint8_t responseBuffer[responseSize];

//   while (_i2c_module->requestFrom((uint8_t)EECX08_ADDRESS, (size_t)responseSize, (bool)true) != responseSize && retries--);
// 
//   responseBuffer[0] = _i2c_module->read();

	uint8_t getresponseSize = 0;

	struct i2c_master_packet packet = {
		.address = EECX08_ADDRESS,
		.data = &responseBuffer,
		.data_length = responseSize,
		.ten_bit_address = false,
		.high_speed      = false,
		.hs_master_code  = 0x0,
	};

	if(i2c_master_read_packet_wait(_i2c_module, &packet) != STATUS_OK) return 0;
// 	uint8_t header = (EECX08_ADDRESS << 1) | 0x01; // set the read bit
// 	if(i2c_master_write_byte(_i2c_module, &header) != STATUS_OK) return 0;
// 	if(i2c_master_read_byte(_i2c_module, &getresponseSize) != STATUS_OK) return 0;

  // make sure length matches
  if (responseBuffer[0] != responseSize) {
    return 0;
  }

//   for (size_t i = 1; _i2c_module->available(); i++) {
//     responseBuffer[i] = _i2c_module->read();
//   }

	//uint8_t tailResponseBuffer[responseSize-1];
// 	packet.data = tailResponseBuffer;
// 	packet.data_length = (responseSize-2);
// 	if(i2c_master_read_packet_wait(_i2c_module, &packet) != STATUS_OK) return 0;
// 	for (size_t i = 1; i < getresponseSize; i++) {
// 		//responseBuffer[i] = _i2c_module->read();
// 		if(i2c_master_read_byte(_i2c_module, &responseBuffer[i]) != STATUS_OK) return 0;
// 	}
	
	// copy the tail over to the response buffer
//	responseBuffer[0] = getresponseSize;
// 	for (size_t i = 1; i < responseSize; i++) {
// 		responseBuffer[i] = tailResponseBuffer[i-1];
// 	}

	// verify CRC
	uint16_t responseCrc = responseBuffer[length + 1] | (responseBuffer[length + 2] << 8);
	if (responseCrc != crc16(responseBuffer, responseSize - 2)) {
	return 0;
	}
  
	memcpy(response, &responseBuffer[1], length);

	return 1;
}

uint16_t crc16(uint8_t* data, size_t length)
{
  if (data == NULL || length == 0) {
    return 0;
  }

  uint16_t crc = 0;

  while (length) {
    uint8_t b = *data;

    for (uint8_t shift = 0x01; shift > 0x00; shift <<= 1) {
      uint8_t dataBit = (b & shift) ? 1 : 0;
      uint8_t crcBit = crc >> 15;

      crc <<= 1;
      
      if (dataBit != crcBit) {
        crc ^= 0x8005;
      }
    }

    length--;
    data++;
  }

  return crc;
}

// #ifdef CRYPTO_WIRE
// ECCX08Class ECCX08(CRYPTO_WIRE, 0x60);
// #else
// ECCX08Class ECCX08(Wire, 0x60);
// #endif