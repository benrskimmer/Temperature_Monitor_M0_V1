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

#ifndef _ECCX08_SELF_SIGNED_CERT_H_
#define _ECCX08_SELF_SIGNED_CERT_H_

#include <inttypes.h>
#include <compiler.h>

// class ECCX08SelfSignedCertClass {
// public:
//   ECCX08SelfSignedCertClass();
//   virtual ~ECCX08SelfSignedCertClass();

void deinitECCX08SelfSignedCert(void);

int beginStorage(int keySlot, int dateAndSignatureSlot, bool newKey);
void endStorage(char* out, size_t* out_length);

int beginReconstruction(int keySlot, int dateAndSignatureSlot);
int endReconstruction(void);

uint8_t* bytes(void);
int length(void);

void sha1(char* sha1Str);

void setIssueYear(int issueYear);
void setIssueMonth(int issueMonth);
void setIssueDay(int issueDay);
void setIssueHour(int issueHour);
void setExpireYears(int expireYears);
void setSerialNumber(const uint8_t* serialNumber, int length);

void SSC_setCountryName(const char *countryName);
//void setCountryName(const String& countryName) { setCountryName(countryName.c_str()); }

void SSC_setStateProvinceName(const char* stateProvinceName);
//void setStateProvinceName(const String& stateProvinceName) { setStateProvinceName(stateProvinceName.c_str()); }

void SSC_setLocalityName(const char* localityName);
//void setLocalityName(const String& localityName) { setLocalityName(localityName.c_str()); }

void SSC_setOrganizationName(const char* organizationName);
//void setOrganizationName(const String& organizationName) { setOrganizationName(organizationName.c_str()); }

void SSC_setOrganizationalUnitName(const char* organizationalUnitName);
//void setOrganizationalUnitName(const String& organizationalUnitName) { setOrganizationalUnitName(organizationalUnitName.c_str()); }

void SSC_setCommonName(const char* commonName);
//void setCommonName(const String& commonName) { setCommonName(commonName.c_str()); }

/*private:*/
  

// private:
//   
// };
// 
// extern ECCX08SelfSignedCertClass ECCX08SelfSignedCert;

#endif
