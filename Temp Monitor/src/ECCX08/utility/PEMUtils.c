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

#include "PEMUtils.h"
#include <string.h>

void base64Encode(uint8_t* in, unsigned int in_length, char* out, size_t* out_length, char* prefix, char* suffix)
{
  static const char* CODES = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

  int b;

  int reserveLength = 4 * ((in_length + 2) / 3) + ((in_length / 3 * 4) / 76) + strlen(prefix) + strlen(suffix);
  
  //char out[reserveLength] = {0};
 // out.reserve(reserveLength);
  uint8_t idx = 0;

  if (prefix) {
    strcpy((char*)&out[idx], prefix);
	idx += strlen(prefix);
  }
  
  for (unsigned int i = 0; i < in_length; i += 3) {
    if (i > 0 && (i / 3 * 4) % 76 == 0) { 
	  strcpy((char*)&out[idx++], "\n");
    }

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
        //out += '=';
		strcpy((char*)&out[idx++], "=");
      }
    } else {
      //out += CODES[b];
	  strcpy((char*)&out[idx++], CODES[b]);
      //out += "==";
	  strcpy((char*)&out[idx], "==");
	  idx += 2;
    }
  }

  if (suffix) {
    //out += suffix;
	strcpy((char*)&out[idx], suffix);
  }

  if(out_length != NULL)
	*out_length = reserveLength;
}

//PEMUtilsClass PEMUtils;
