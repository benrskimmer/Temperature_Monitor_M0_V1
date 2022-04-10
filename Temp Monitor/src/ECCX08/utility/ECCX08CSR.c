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

#include <stdio.h>
#include <string.h>

#include "ASN1Utils.h"
#include "PEMUtils.h"

#include "ECCX08CSR.h"

// ECCX08CSRClass()
// {
// }
// 
// ~ECCX08CSRClass()
// {
// }

int _slot;

char _countryName[64];
char _stateProvinceName[64];
char _localityName[64];
char _organizationName[64];
char _organizationalUnitName[64];
char _commonName[64];

uint8_t _publicKey[64];

int begin(int slot, bool newPrivateKey)
{
  _slot = slot;

  if (newPrivateKey) {
    if (!ECCX08_generatePrivateKey(slot, _publicKey)) {
      return 0;
    }
  } else {
    if (!ECCX08_generatePublicKey(slot, _publicKey)) {
      return 0;
    }
  }

  return 1;
}

char* end(void)
{
  int versionLen = versionLength();
  int subjectLen = issuerOrSubjectLength(_countryName,
                                                    _stateProvinceName,
                                                    _localityName,
                                                    _organizationName,
                                                    _organizationalUnitName,
                                                    _commonName);
  int subjectHeaderLen = sequenceHeaderLength(subjectLen);
  int publicKeyLen = publicKeyLength();

  int csrInfoLen = versionLen + subjectHeaderLen + subjectLen + publicKeyLen + 2;
  int csrInfoHeaderLen = sequenceHeaderLength(csrInfoLen);

  uint8_t csrInfo[csrInfoHeaderLen + csrInfoLen];
  uint8_t* out = csrInfo;

  appendSequenceHeader(csrInfoLen, out);
  out += csrInfoHeaderLen;

  // version
  appendVersion(0x00, out);
  out += versionLen;

  // subject
  appendSequenceHeader(subjectLen, out);
  out += subjectHeaderLen;
  appendIssuerOrSubject(_countryName,
                                  _stateProvinceName,
                                  _localityName,
                                  _organizationName,
                                  _organizationalUnitName,
                                  _commonName, out);
  out += subjectLen;

  // public key
  appendPublicKey(_publicKey, out);
  out += publicKeyLen;

  // terminator
  *out++ = 0xa0;
  *out++ = 0x00;

  uint8_t csrInfoSha256[64];
  uint8_t signature[64];

  if (!ECCX08_beginSHA256()) {
    return "";
  }

  for (int i = 0; i < (csrInfoHeaderLen + csrInfoLen); i += 64) {
    int chunkLength = (csrInfoHeaderLen + csrInfoLen) - i;

    if (chunkLength > 64) {
      chunkLength = 64;
    }

    if (chunkLength == 64) {
      if (!ECCX08_updateSHA256(&csrInfo[i])) {
        return "";
      }
    } else {
      if (!ECCX08_endSHA256Data(&csrInfo[i], chunkLength, csrInfoSha256)) {
        return "";
      }
    }
  }

  if (!ECCX08_ecSign(_slot, csrInfoSha256, signature)) {
    return "";
  }

  int signatureLen = signatureLength(signature);
  int csrLen = csrInfoHeaderLen + csrInfoLen + signatureLen;
  int csrHeaderLen = sequenceHeaderLength(csrLen);

  uint8_t csr[csrLen + csrHeaderLen];
  out = csr;

  appendSequenceHeader(csrLen, out);
  out += csrHeaderLen;

  // info
  memcpy(out, csrInfo, csrInfoHeaderLen + csrInfoLen);
  out += (csrInfoHeaderLen + csrInfoLen);

  // signature
  appendSignature(signature, out);
  out += signatureLen;

  char* out_str;
  base64Encode(csr, csrLen + csrHeaderLen, out_str, NULL,"-----BEGIN CERTIFICATE REQUEST-----\n", "\n-----END CERTIFICATE REQUEST-----\n");
  return out_str;
}

void setCountryName(const char *countryName)
{
  strcpy(_countryName, countryName);
}

void setStateProvinceName(const char* stateProvinceName)
{
  strcpy(_stateProvinceName, stateProvinceName);
}

void setLocalityName(const char* localityName)
{
  strcpy(_localityName, localityName);
}

void setOrganizationName(const char* organizationName)
{
  strcpy(_organizationName, organizationName);
}

void setOrganizationalUnitName(const char* organizationalUnitName)
{
  strcpy(_organizationalUnitName, organizationalUnitName);
}

void setCommonName(const char* commonName)
{
  strcpy(_commonName, commonName);
}


//ECCX08CSRClass ECCX08CSR;
