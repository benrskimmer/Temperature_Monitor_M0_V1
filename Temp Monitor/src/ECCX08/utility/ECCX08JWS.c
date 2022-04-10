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

#include "ECCX08/ECCX08.h"

#include "ASN1Utils.h"
#include "PEMUtils.h"

#include "ECCX08JWS.h"

#include <string.h>

void base64urlEncode(uint8_t* in, unsigned int in_length, char* out, size_t* out_length)
{
  static const char* CODES = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_=";

  int b;
  
  int reserveLength = 4 * ((in_length + 2) / 3);
  //out.reserve(reserveLength);
  //char out[reserveLength] = {0};
  uint8_t idx = 0;

  for (unsigned int i = 0; i < in_length; i += 3) {
    b = (in[i] & 0xFC) >> 2;
    //out += CODES[b];
	strcpy((char*)&out[idx++], CODES[b]);

    b = (in[i] & 0x03) << 4;
    if (i + 1 < in_length) {
      b |= (in[i + 1] & 0xF0) >> 4;
      //out += CODES[b];
	  strcpy((char*)&out[idx++], CODES[b]);
      b = (in[i + 1] & 0x0F) << 2;
      if (i + 2 < in_length) {
         b |= (in[i + 2] & 0xC0) >> 6;
         //out += CODES[b];
		 strcpy((char*)&out[idx++], CODES[b]);
         b = in[i + 2] & 0x3F;
         //out += CODES[b];
		 strcpy((char*)&out[idx++], CODES[b]);
      } else {
        //out += CODES[b];
		strcpy((char*)&out[idx++], CODES[b]);
      }
    } else {
      //out += CODES[b];
	  strcpy((char*)&out[idx++], CODES[b]);
    }
  }

//   while (out.lastIndexOf('=') != -1) {
//     out.remove(out.length() - 1);
//   }

  for(uint8_t i = 0; i < reserveLength; i++)
  {
	  if(out[i] == '=')
		  out[(reserveLength-1)-i] = 0; // delete end character for each '='
  }

  if(out_length != NULL)
	*out_length = reserveLength;
}

// ECCX08JWSClass::ECCX08JWSClass()
// {
// }
// 
// ECCX08JWSClass::~ECCX08JWSClass()
// {
// }

void publicKey(int slot, bool newPrivateKey, char* result)
{
  if (slot < 0 || slot > 8) {
	result = "";
    return;
  }

  uint8_t publicKey[64];

  if (newPrivateKey) {
    if (!ECCX08_generatePrivateKey(slot, publicKey)) {
	  result = "";
      return;
    }
  } else {
    if (!ECCX08_generatePublicKey(slot, publicKey)) {
	  result = "";
      return;
    }
  }

  int length = publicKeyLength();
  uint8_t out[length];

  appendPublicKey(publicKey, out);

  base64Encode(out, length, result, NULL, "-----BEGIN PUBLIC KEY-----\n", "\n-----END PUBLIC KEY-----\n");
}

void sign(int slot, const char* header, const char* payload, char* result, size_t* result_length)
{
  if (slot < 0 || slot > 8) {
	  result = "";
	  return;
  }

  char* encodedHeader;
  char* encodedPayload;
  base64urlEncode((uint8_t*)header, strlen(header), encodedHeader, NULL);
  base64urlEncode((uint8_t*)payload, strlen(payload), encodedPayload, NULL);

  char* toSign[strlen(encodedHeader) + 1 + strlen(encodedPayload)];
  memset(toSign, '\0', strlen(encodedHeader) + 1 + strlen(encodedPayload));
  //toSign.reserve(encodedHeader.length() + 1 + encodedPayload.length());
  
  uint16_t idx = 0;

//   toSign += encodedHeader;
//   toSign += '.';
//   toSign += encodedPayload;
  strcpy((char*)&toSign[idx], encodedHeader);
  idx += strlen(encodedHeader);
  strcpy((char*)&toSign[idx++], ".");
  strcpy((char*)&toSign[idx], encodedPayload);

  uint8_t toSignSha256[32] = {0};
  uint8_t signature[64] = {0};

  if (!ECCX08_beginSHA256()) {
	result = "";
    return;
  }

  for (unsigned int i = 0; i < strlen(toSign); i += 64) {
    int chunkLength = strlen(toSign) - i;

    if (chunkLength > 64) {
      chunkLength = 64;
    }

    if (chunkLength == 64) {
      if (!ECCX08_updateSHA256((const uint8_t*)toSign + i)) {
		result = "";
        return;
      }
    } else {
      if (!ECCX08_endSHA256Data((const uint8_t*)toSign + i, chunkLength, toSignSha256)) {
		result = "";
        return;
      }
    }
  }

  if (!ECCX08_ecSign(slot, toSignSha256, signature)) {
	result = "";
    return;
  }

  char encodedSignature[(4 * ((sizeof(signature)) + 2) / 3) + 1] = {0};
  base64urlEncode(signature, sizeof(signature), encodedSignature, NULL);

  //String result;
  //result.reserve(toSign.length() + 1 + encodedSignature.length());
  
  if(result_length != NULL)
	*result_length = strlen(toSign) + 1 + strlen(encodedSignature);

//   result += toSign;
//   result += '.';
//   result += encodedSignature;
  idx = 0;
  strcpy((char*)&result[idx], toSign);
  idx += strlen(toSign);
  strcpy((char*)&result[idx++], ".");
  strcpy((char*)&result[idx], encodedSignature);
}

// String ECCX08JWSClass::sign(int slot, const String& header, const String& payload)
// {
//   return sign(slot, header.c_str(), payload.c_str());
// }
// 
// ECCX08JWSClass ECCX08JWS;
