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

#ifndef _ASN1_UTILS_H_
#define _ASN1_UTILS_H_

//#include <Arduino.h>
#include <inttypes.h>
#include <compiler.h>

#define ASN1_INTEGER           0x02
#define ASN1_BIT_STRING        0x03
#define ASN1_NULL              0x05
#define ASN1_OBJECT_IDENTIFIER 0x06
#define ASN1_PRINTABLE_STRING  0x13
#define ASN1_SEQUENCE          0x30
#define ASN1_SET               0x31

// class ASN1UtilsClass {
// public:
int versionLength(void);

int issuerOrSubjectLength(const char* countryName,
                const char* stateProvinceName,
                const char* localityName,
                const char* organizationName,
                const char* organizationalUnitName,
                const char* commonName);

int publicKeyLength(void);

int signatureLength(uint8_t* signature);

int serialNumberLength(const uint8_t* serialNumber, int length);

int sequenceHeaderLength(int length);

void appendVersion(int version, uint8_t* out);

void appendIssuerOrSubject(const char* countryName,
                            const char* stateProvinceName,
                            const char* localityName,
                            const char* organizationName,
                            const char* organizationalUnitName,
                            const char* commonName,
                            uint8_t* out);

int appendPublicKey(const uint8_t* publicKey, uint8_t* out);

int appendSignature(const uint8_t* signature, uint8_t* out);

int appendSerialNumber(const uint8_t* serialNumber, int length, uint8_t* out);

int appendName(char* name, int type, uint8_t* out);

int appendSequenceHeader(int length, uint8_t* out);

int appendDate(int year, int month, int day, int hour, int minute, int second, uint8_t* out);

int appendEcdsaWithSHA256(uint8_t* out);
//};

//extern ASN1UtilsClass ASN1Utils;

#endif
