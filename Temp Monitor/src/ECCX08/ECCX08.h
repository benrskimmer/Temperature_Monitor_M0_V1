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

#ifndef _ECCX08_H_
#define _ECCX08_H_

// #include <Arduino.h>
// #include <Wire.h>
#include <inttypes.h>
#include <compiler.h>
#include <c_types.h>
#include "i2c_master.h"

#define EECX08_ADDRESS 0x60 // arduino uses 0x60

void ECCX08_init(struct i2c_master_module* module);

BOOL ECCX08_begin(void);
//virtual ~ECCX08Class();

// int begin();
// int begin(uint8_t i2cAddress);

void ECCX08_end(void);

int ECCX08_serialNumber(uint8_t* sn);
//String ECCX08_getSerialNumber(void);

long ECCX08_random(long max);
long ECCX08_random_range(long min, long max);
int ECCX08_random_array(uint8_t* data, size_t length);

int ECCX08_generatePrivateKey(int slot, uint8_t* publicKey);
int ECCX08_generatePublicKey(int slot, uint8_t* publicKey);

int ECCX08_ecdsaVerify(uint8_t* message, uint8_t* signature, uint8_t* pubkey);
int ECCX08_ecSign(int slot, uint8_t* message, uint8_t* signature);

int ECCX08_beginSHA256(void);
int ECCX08_updateSHA256(uint8_t* data); // 64 bytes
int ECCX08_endSHA256(uint8_t* result);
int ECCX08_endSHA256Data(uint8_t* data, int length, uint8_t* result);

int ECCX08_readSlot(int slot, uint8_t* data, int length);
int ECCX08_writeSlot(int slot, uint8_t* data, int length);

int ECCX08_locked(void);
int ECCX08_writeConfiguration(uint8_t* data);
int ECCX08_readConfiguration(uint8_t* data);
int ECCX08_lock(void);


#endif
