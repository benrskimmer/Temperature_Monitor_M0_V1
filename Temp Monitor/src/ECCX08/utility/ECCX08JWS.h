/*
  This file is part of the ArduinoECCX08 library.
  Copyright (c) 2019 Arduino SA. All rights reserved.

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

#ifndef _ECCX08_JWS_H_
#define _ECCX08_JWS_H_

// #include <Arduino.h>
// 
// class ECCX08JWSClass {
// public:
//   ECCX08JWSClass();
//   virtual ~ECCX08JWSClass();

void publicKey(int slot, bool newPrivateKey, char* result);

void sign(int slot, const char* header, const char* payload, char* result, size_t* result_length);
//  char* sign(int slot, String& header, String& payload);
// };
// 
// extern ECCX08JWSClass ECCX08JWS;

#endif
