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

#include "ECCX08/ArduinoECCX08.h"

//extern "C" {
#include "sha1.h"
//}
#include "ECCX08/utility/ASN1Utils.h"
#include "ECCX08/utility/PEMUtils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ECCX08SelfSignedCert.h"

static const uint8_t DEFAULT_SERIAL_NUMBER[] = { 0x01 };

int _keySlot;
int _dateAndSignatureSlot;

char _countryName[64];
char _stateProvinceName[64];
char _localityName[64];
char _organizationName[64];
char _organizationalUnitName[64];
char _commonName[64];

uint8_t _temp[72];
const uint8_t* _serialNumber = DEFAULT_SERIAL_NUMBER;
int _serialNumberLength = sizeof(DEFAULT_SERIAL_NUMBER);

uint8_t* _bytes = NULL;
int _length = 0;


struct __attribute__((__packed__)) CompressedCert {
  uint8_t signature[64];
  struct {
    uint8_t year:5;
    uint8_t month:4;
    uint8_t day:5;
    uint8_t hour:5;
    uint8_t expires:5;
  } dates;
  uint8_t unused[5];
};


	
// Private function prototypes
int buildCert(bool buildSignature);
int certInfoLength();
void appendCertInfo(uint8_t publicKey[], uint8_t buffer[], int length);



	
// Public functions

// ECCX08SelfSignedCertClass() :
//   _serialNumber(DEFAULT_SERIAL_NUMBER),
//   _serialNumberLength(sizeof(DEFAULT_SERIAL_NUMBER)),
//   _bytes(NULL),
//   _length(0)
// {
// }

// ~ECCX08SelfSignedCertClass()
void deinitECCX08SelfSignedCert(void)
{
  if (_bytes) {
    free(_bytes);
    _bytes = NULL;
  }
}

int beginStorage(int keySlot, int dateAndSignatureSlot, bool newKey)
{
  if (keySlot < 0 || keySlot > 8) {
    return 0;
  }

  if (dateAndSignatureSlot < 8 || dateAndSignatureSlot > 15) {
    return 0;
  }

  _keySlot = keySlot;
  _dateAndSignatureSlot = dateAndSignatureSlot;

  uint8_t publicKey[64];

  if (newKey && !ECCX08_generatePrivateKey(_keySlot, publicKey)) {
    return 0;
  }

  return 1;
}

void endStorage(char* out, size_t* out_length)
{
  if (!buildCert(true)) {
	*out = 0;
	if(out_length != NULL) *out_length = 0;
    return;
  }

  base64Encode(_bytes, _length, out, out_length,  "-----BEGIN CERTIFICATE-----\n", "\n-----END CERTIFICATE-----\n");
}

int beginReconstruction(int keySlot, int dateAndSignatureSlot)
{
  if (keySlot < 0 || keySlot > 8) {
    return 0;
  }

  if (dateAndSignatureSlot < 8 || dateAndSignatureSlot > 15) {
    return 0;
  }

  _keySlot = keySlot;
  _dateAndSignatureSlot = dateAndSignatureSlot;

  if (!ECCX08_readSlot(_dateAndSignatureSlot, _temp, sizeof(_temp))) {
    return 0;
  }

  return 1;
}

int endReconstruction(void)
{
  if (!buildCert(false)) {
    return 0;
  }

  return 1;
}

uint8_t* bytes(void)
{
  return _bytes;
}

int length(void)
{
  return _length;
}

void sha1(char* sha1Str)
{
  char result[20 + 1];

  SHA1(result, (const char*)_bytes, _length);

//  char sha1Str[40 + 1];

//  sha1Str.reserve(40);

  for (int i = 0; i < 20; i++) {
    uint8_t b = result[i];

//     if (b < 16) {
//       sha1Str += '0';
//     }
//     sha1Str += String(b, HEX);
	
	sprintf((char*)(sha1Str+2*i), "%02x", b);
  }
  
  *(char*)(sha1Str+40) = '\0'; // Null terminate last character of 41 byte string
//  return sha1Str;
}

void setIssueYear(int issueYear)
{
  struct CompressedCert* compressedCert = (struct CompressedCert*)_temp;

  compressedCert->dates.year = (issueYear - 2000);
}

void setIssueMonth(int issueMonth)
{
  struct CompressedCert* compressedCert = (struct CompressedCert*)_temp;

  compressedCert->dates.month = issueMonth;
}

void setIssueDay(int issueDay)
{
  struct CompressedCert* compressedCert = (struct CompressedCert*)_temp;

  compressedCert->dates.day = issueDay;
}

void setIssueHour(int issueHour)
{
  struct CompressedCert* compressedCert = (struct CompressedCert*)_temp;

  compressedCert->dates.hour = issueHour;
}

void setExpireYears(int expireYears)
{
  struct CompressedCert* compressedCert = (struct CompressedCert*)_temp;

  compressedCert->dates.expires = expireYears;
}

void setSerialNumber(const uint8_t* serialNumber, int length)
{
  _serialNumber = serialNumber;
  _serialNumberLength = length;
}

void SSC_setCountryName(const char *countryName)
{
  strcpy(_countryName, countryName);
}

void SSC_setStateProvinceName(const char* stateProvinceName)
{
  strcpy(_stateProvinceName, stateProvinceName);
}

void SSC_setLocalityName(const char* localityName)
{
  strcpy(_localityName, localityName);
}

void SSC_setOrganizationName(const char* organizationName)
{
  strcpy(_organizationName, organizationName);
}

void SSC_setOrganizationalUnitName(const char* organizationalUnitName)
{
  strcpy(_organizationalUnitName, organizationalUnitName);
}

void SSC_setCommonName(const char* commonName)
{
  strcpy(_commonName ,commonName);
}

int buildCert(bool buildSignature)
{
  uint8_t publicKey[64];

  if (!ECCX08_generatePublicKey(_keySlot, publicKey)) {
    return 0;
  }

  int certInfoLen = certInfoLength();
  int certInfoHeaderLen = sequenceHeaderLength(certInfoLen);

  uint8_t certInfo[certInfoLen + certInfoHeaderLen];

  appendCertInfo(publicKey, certInfo, certInfoLen);
  
  if (buildSignature) {
    uint8_t certInfoSha256[64];

    memset(certInfoSha256, 0x00, sizeof(certInfoSha256));

    if (!ECCX08_beginSHA256()) {
      return 0;
    }

    for (int i = 0; i < (certInfoHeaderLen + certInfoLen); i += 64) {
      int chunkLength = (certInfoHeaderLen + certInfoLen) - i;

      if (chunkLength > 64) {
        chunkLength = 64;
      }

      if (chunkLength == 64) {
        if (!ECCX08_updateSHA256(&certInfo[i])) {
          return 0;
        }
      } else {
        if (!ECCX08_endSHA256Data(&certInfo[i], chunkLength, certInfoSha256)) {
          return 0;
        }
      }
    }

    if (!ECCX08_ecSign(_keySlot, certInfoSha256, _temp)) {
      return 0;
    }

    if (!ECCX08_writeSlot(_dateAndSignatureSlot, _temp, sizeof(_temp))) {
      return 0;
    }
  }

  int signatureLen = signatureLength(_temp);

  int certDataLen = certInfoLen + certInfoHeaderLen + signatureLen;
  int certDataHeaderLen = sequenceHeaderLength(certDataLen);

  _length = certDataLen + certDataHeaderLen;
  _bytes = (uint8_t*)realloc(_bytes, _length);

  if (!_bytes) {
    _length = 0;
    return 0;
  }

  uint8_t* out = _bytes;

  out += appendSequenceHeader(certDataLen, out);

  memcpy(out, certInfo, certInfoHeaderLen + certInfoLen);
  out += (certInfoHeaderLen + certInfoLen);

  // signature
  out += appendSignature(_temp, out);

  return 1;
}

int certInfoLength()
{
  struct CompressedCert* compressedCert = (struct CompressedCert*)_temp;

  int year = (compressedCert->dates.year + 2000);
  int expireYears = compressedCert->dates.expires;

  int datesSize = 30;

  if (year > 2049) {
    // two more bytes for GeneralizedTime
    datesSize += 2;
  }

  if ((year + expireYears) > 2049) {
    // two more bytes for GeneralizedTime
    datesSize += 2;
  }

  int serialNumberLen = serialNumberLength(_serialNumber, _serialNumberLength);

  int issuerAndSubjectLen = issuerOrSubjectLength(_countryName,
                                                            _stateProvinceName,
                                                            _localityName,
                                                            _organizationName,
                                                            _organizationalUnitName,
                                                            _commonName);

  int issuerAndSubjectHeaderLen = sequenceHeaderLength(issuerAndSubjectLen);


  int publicKeyLen = publicKeyLength();
  

  int certInfoLen = 5 + serialNumberLen + 12 + 
                    2 * (issuerAndSubjectHeaderLen + issuerAndSubjectLen) + 
                    (datesSize + 2) + publicKeyLen + 4;

  return certInfoLen;
}

void appendCertInfo(uint8_t publicKey[], uint8_t buffer[], int length)
{
  struct CompressedCert* compressedCert = (struct CompressedCert*)_temp;
  uint8_t* out = buffer;

  // dates
  int year = (compressedCert->dates.year + 2000);
  int month = compressedCert->dates.month;
  int day = compressedCert->dates.day;
  int hour = compressedCert->dates.hour;
  int expireYears = compressedCert->dates.expires;

  out += appendSequenceHeader(length, out);

  // version
  *out++ = 0xA0;
  *out++ = 0x03;
  *out++ = 0x02;
  *out++ = 0x01;
  *out++ = 0x02;

  // serial number
  out += appendSerialNumber(_serialNumber, _serialNumberLength, out);

  out += appendEcdsaWithSHA256(out);

  // issuer
  int issuerAndSubjectLen = issuerOrSubjectLength(_countryName,
                                                            _stateProvinceName,
                                                            _localityName,
                                                            _organizationName,
                                                            _organizationalUnitName,
                                                            _commonName);

  out += appendSequenceHeader(issuerAndSubjectLen, out);
  appendIssuerOrSubject(_countryName,
                                  _stateProvinceName,
                                  _localityName,
                                  _organizationName,
                                  _organizationalUnitName,
                                  _commonName, out);
  out += issuerAndSubjectLen;

  *out++ = ASN1_SEQUENCE;
  *out++ = 30 + ((year > 2049) ? 2 : 0) + (((year + expireYears) > 2049) ? 2 : 0);
  out += appendDate(year, month, day, hour, 0, 0, out);
  out += appendDate(year + expireYears, month, day, hour, 0, 0, out);

  // subject
  out += appendSequenceHeader(issuerAndSubjectLen, out);
  appendIssuerOrSubject(_countryName,
                                  _stateProvinceName,
                                  _localityName,
                                  _organizationName,
                                  _organizationalUnitName,
                                  _commonName, out);
  out += issuerAndSubjectLen;

  // public key
  out += appendPublicKey(publicKey, out);

  // null sequence
  *out++ = 0xA3;
  *out++ = 0x02;
  *out++ = 0x30;
  *out++ = 0x00;
}

//ECCX08SelfSignedCertClass ECCX08SelfSignedCert;
