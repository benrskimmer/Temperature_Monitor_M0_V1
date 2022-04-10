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

#include "ASN1Utils.h"
#include <string.h>

int versionLength(void)
{
  return 3;
}

int issuerOrSubjectLength(const char* countryName,
                                          const char* stateProvinceName,
                                          const char* localityName,
                                          const char* organizationName,
                                          const char* organizationalUnitName,
                                          const char* commonName)
{
  int length                       = 0;
  int countryNameLength            = strlen(countryName);
  int stateProvinceNameLength      = strlen(stateProvinceName);
  int localityNameLength           = strlen(localityName);
  int organizationNameLength       = strlen(organizationName);
  int organizationalUnitNameLength = strlen(organizationalUnitName);
  int commonNameLength             = strlen(commonName);

  if (countryNameLength) {
    length += (11 + countryNameLength);
  }

  if (stateProvinceNameLength) {
    length += (11 + stateProvinceNameLength);
  }

  if (localityNameLength) {
    length += (11 + localityNameLength);
  }

  if (organizationNameLength) {
    length += (11 + organizationNameLength);
  }

  if (organizationalUnitNameLength) {
    length += (11 + organizationalUnitNameLength);
  }

  if (commonNameLength) {
    length += (11 + commonNameLength);
  }

  return length;
}

int publicKeyLength(void)
{
  return (2 + 2 + 9 + 10 + 4 + 64);
}

int signatureLength(uint8_t* signature)
{
  const uint8_t* r = &signature[0];
  const uint8_t* s = &signature[32];

  int rLength = 32;
  int sLength = 32;

  while (*r == 0x00 && rLength > 1) {
    r++;
    rLength--;
  }

  if (*r & 0x80) {
    rLength++;
  }

  while (*s == 0x00 && sLength > 1) {
    s++;
    sLength--;
  }

  if (*s & 0x80) {
    sLength++;
  }

  return (21 + rLength + sLength);
}

int serialNumberLength(const uint8_t* serialNumber, int length)
{
  while (*serialNumber == 0 && length > 1) {
    serialNumber++;
    length--;
  }

  if (*serialNumber & 0x80) {
    length++;
  }

  return (2 + length);
}

int sequenceHeaderLength(int length)
{
  if (length > 255) {
    return 4;
  } else if (length > 127) {
    return 3;
  } else {
    return 2;
  }
}

void appendVersion(int version, uint8_t* out)
{
  out[0] = ASN1_INTEGER;
  out[1] = 0x01;
  out[2] = version;
}

void appendIssuerOrSubject(const char* countryName,
                                            const char* stateProvinceName,
                                            const char* localityName,
                                            const char* organizationName,
                                            const char* organizationalUnitName,
                                            const char* commonName,
                                            uint8_t* out)
{
  if (strlen(countryName) > 0) {
    out += appendName(countryName, 0x06, out);
  }

  if (strlen(stateProvinceName) > 0) {
    out += appendName(stateProvinceName, 0x08, out);
  }

  if (strlen(localityName) > 0) {
    out += appendName(localityName, 0x07, out);
  }

  if (strlen(organizationName) > 0) {
    out += appendName(organizationName, 0x0a, out);
  }

  if (strlen(organizationalUnitName) > 0) {
    out += appendName(organizationalUnitName, 0x0b, out);
  }

  if (strlen(commonName) > 0) {
    out += appendName(commonName, 0x03, out);
  }
}

int appendPublicKey(const uint8_t* publicKey, uint8_t* out)
{
  int subjectPublicKeyDataLength = 2 + 9 + 10 + 4 + 64;

  // subject public key
  *out++ = ASN1_SEQUENCE;
  *out++ = (subjectPublicKeyDataLength) & 0xff;

  *out++ = ASN1_SEQUENCE;
  *out++ = 0x13;

  // EC public key
  *out++ = ASN1_OBJECT_IDENTIFIER;
  *out++ = 0x07;
  *out++ = 0x2a;
  *out++ = 0x86;
  *out++ = 0x48;
  *out++ = 0xce;
  *out++ = 0x3d;
  *out++ = 0x02;
  *out++ = 0x01;

  // PRIME 256 v1
  *out++ = ASN1_OBJECT_IDENTIFIER;
  *out++ = 0x08;
  *out++ = 0x2a;
  *out++ = 0x86;
  *out++ = 0x48;
  *out++ = 0xce;
  *out++ = 0x3d;
  *out++ = 0x03;
  *out++ = 0x01;
  *out++ = 0x07;

  *out++ = 0x03;
  *out++ = 0x42;
  *out++ = 0x00;
  *out++ = 0x04;

  memcpy(out, publicKey, 64);

  return (2 + subjectPublicKeyDataLength);
}

int appendSignature(const uint8_t* signature, uint8_t* out)
{
  // signature algorithm
  *out++ = ASN1_SEQUENCE;
  *out++ = 0x0a;
  *out++ = ASN1_OBJECT_IDENTIFIER;
  *out++ = 0x08;

  // ECDSA with SHA256
  *out++ = 0x2a;
  *out++ = 0x86;
  *out++ = 0x48;
  *out++ = 0xce;
  *out++ = 0x3d;
  *out++ = 0x04;
  *out++ = 0x03;
  *out++ = 0x02;

  const uint8_t* r = &signature[0];
  const uint8_t* s = &signature[32];

  int rLength = 32;
  int sLength = 32;

  while (*r == 0 && rLength > 1) {
    r++;
    rLength--;
  }

  while (*s == 0 && sLength > 1) {
    s++;
    sLength--;
  }

  if (*r & 0x80) {
    rLength++;  
  }

  if (*s & 0x80) {
    sLength++;
  }

  *out++ = ASN1_BIT_STRING;
  *out++ = (rLength + sLength + 7);
  *out++ = 0;

  *out++ = ASN1_SEQUENCE;
  *out++ = (rLength + sLength + 4);

  *out++ = ASN1_INTEGER;
  *out++ = rLength;
  if (*r & 0x80) {
    *out++ = 0;
    rLength--;
  }
  memcpy(out, r, rLength);
  out += rLength;

  if (*r & 0x80) {
    rLength++;
  }

  *out++ = ASN1_INTEGER;
  *out++ = sLength;
  if (*s & 0x80) {
    *out++ = 0;
    sLength--;
  }
  memcpy(out, s, sLength);
  out += sLength;
  
  if (*s & 0x80) {
    sLength++;
  }
  
  return (21 + rLength + sLength);
}

int appendSerialNumber(const uint8_t* serialNumber, int length, uint8_t* out)
{
  while (*serialNumber == 0 && length > 1) {
    serialNumber++;
    length--;
  }

  if (*serialNumber & 0x80) {
    length++;  
  }

  *out++ = ASN1_INTEGER;
  *out++ = length;

  if (*serialNumber & 0x80) {
    *out++ = 0x00;
    length--;
  }

  memcpy(out, serialNumber, length);
  
  if (*serialNumber & 0x80) {
    length++;
  }

  return (2 + length);
}

int appendName(char* name, int type, uint8_t* out)
{
  int nameLength = strlen(name);

  *out++ = ASN1_SET;
  *out++ = nameLength + 9;

  *out++ = ASN1_SEQUENCE;
  *out++ = nameLength + 7;

  *out++ = ASN1_OBJECT_IDENTIFIER;
  *out++ = 0x03;
  *out++ = 0x55;
  *out++ = 0x04;
  *out++ = type;

  *out++ = ASN1_PRINTABLE_STRING;
  *out++ = nameLength;
  memcpy(out, name, nameLength);

  return (nameLength + 11);
}

int appendSequenceHeader(int length, uint8_t* out)
{
  *out++ = ASN1_SEQUENCE;
  if (length > 255) {
    *out++ = 0x82;
    *out++ = (length >> 8) & 0xff;
  } else if (length > 127) {
    *out++ = 0x81;
  }
  *out++ = (length) & 0xff;

  if (length > 255) {
    return 4;
  } else if (length > 127) {
    return 3;
  } else {
    return 2;
  }
}

int appendDate(int year, int month, int day, int hour, int minute, int second, uint8_t* out)
{
  bool useGeneralizedTime = (year > 2049);

  if (useGeneralizedTime) {
    *out++ = 0x18;
    *out++ = 0x0f;
    *out++ = '0' + (year / 1000);
    *out++ = '0' + ((year % 1000) / 100);
    *out++ = '0' + ((year % 100) / 10);
    *out++ = '0' + (year % 10);
  } else {
    year -= 2000;

    *out++ = 0x17;
    *out++ = 0x0d;
    *out++ = '0' + (year / 10);
    *out++ = '0' + (year % 10);
  }
  *out++ = '0' + (month / 10);
  *out++ = '0' + (month % 10);
  *out++ = '0' + (day / 10);
  *out++ = '0' + (day % 10);
  *out++ = '0' + (hour / 10);
  *out++ = '0' + (hour % 10);
  *out++ = '0' + (minute / 10);
  *out++ = '0' + (minute % 10);
  *out++ = '0' + (second / 10);
  *out++ = '0' + (second % 10);
  *out++ = 0x5a; // UTC

  return (useGeneralizedTime ? 17 : 15);
}

int appendEcdsaWithSHA256(uint8_t* out)
{
  *out++ = ASN1_SEQUENCE;
  *out++ = 0x0A;
  *out++ = ASN1_OBJECT_IDENTIFIER;
  *out++ = 0x08;
  *out++ = 0x2A;
  *out++ = 0x86;
  *out++ = 0x48;
  *out++ = 0xCE;
  *out++ = 0x3D;
  *out++ = 0x04;
  *out++ = 0x03;
  *out++ = 0x02;

  return 12;
}

//ASN1UtilsClass ASN1Utils;
