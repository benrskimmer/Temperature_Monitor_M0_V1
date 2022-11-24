/* Arduino SSD1306Ascii Library
 * Copyright (C) 2015 by William Greiman
 *
 * This file is part of the Arduino SSD1306Ascii Library
 *
 * This Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Arduino SSD1306Ascii Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "SSD1306Ascii.h"

#include <stdint.h>
#include <stddef.h>
#include <i2c_master.h>

///////////////////// PRIVATE VARIABLES /////////////////////

uint8_t m_col;            // Cursor column.
uint8_t m_row;            // Cursor RAM row.
uint8_t m_displayWidth;   // Display width.
uint8_t m_displayHeight;  // Display height.
uint8_t m_colOffset;      // Column offset RAM to SEG.
uint8_t m_letterSpacing;  // Letter-spacing in pixels.
#if INCLUDE_SCROLLING
uint8_t m_startLine;      // Top line of display
uint8_t m_pageOffset;     // Top page of RAM window.
uint8_t m_scrollMode;  // Scroll mode for newline.
#endif  // INCLUDE_SCROLLING
uint8_t m_skip;
const uint8_t* m_font;  // Current font.
uint8_t m_invertMask;  // font invert mask
uint8_t m_magFactor;   // Magnification factor.
uint8_t m_nData;
// I2C peripheral information
extern struct i2c_master_module i2c_master_instance;
uint8_t screen_address = 0x3C;

///////////////////// PRIVATE FUNCTIONS /////////////////////
uint16_t fontSize(void);
void writeDisplay(uint8_t b, uint8_t mode);
static void _I2CWrite(uint8_t byte);

///////////////////// PRIVATE FUNCTIONS /////////////////////


int oledInit(const DevType* dev)
{
	m_scrollMode = INITIAL_SCROLL_MODE;
	m_skip = 0;
	m_font = NULL;
	m_invertMask = 0;
	m_magFactor = 1;
	
	m_col = 0;
	m_row = 0;
	#ifdef __AVR__
	const uint8_t* table = (const uint8_t*)pgm_read_word(&dev->initcmds);
	#else  // __AVR__
	const uint8_t* table = dev->initcmds;
	#endif  // __AVR
	uint8_t size = readFontByte(&dev->initSize);
	m_displayWidth = readFontByte(&dev->lcdWidth);
	m_displayHeight = readFontByte(&dev->lcdHeight);
	m_colOffset = readFontByte(&dev->colOffset);
	for (uint8_t i = 0; i < size; i++) {
		SSD1306_WriteCmd(readFontByte(table + i));
	}
	SSD1306_clearNoArg();
}

void SSD1306_begin(uint8_t i2cAddr) {
	// A version of the Adafruit128x32 display with 64 lines of buffer.
	static const DevType MEM_TYPE Adafruit128x32x64 = { Adafruit128x32init, sizeof(Adafruit128x32init), 128, 64, 0 };
		
	#if OPTIMIZE_I2C
	m_nData = 0;
	#endif  // OPTIMIZE_I2C
	screen_address = i2cAddr;
	//m_i2cAddr = i2cAddr;
	oledInit(&Adafruit128x32x64);
}

//------------------------------------------------------------------------------
uint8_t SSD1306_charWidth(uint8_t c) {
  if (!m_font) {
    return 0;
  }
  uint8_t first = readFontByte(m_font + FONT_FIRST_CHAR);
  uint8_t count = readFontByte(m_font + FONT_CHAR_COUNT);
  if (c < first || c >= (first + count)) {
    return 0;
  }
  if (fontSize() > 1) {
    // Proportional font.
    return m_magFactor*readFontByte(m_font + FONT_WIDTH_TABLE + c - first);
  }
  // Fixed width font.
  return m_magFactor*readFontByte(m_font + FONT_WIDTH);
}
//------------------------------------------------------------------------------
void SSD1306_clearNoArg(void) {
#if INCLUDE_SCROLLING
  m_pageOffset = 0;
  SSD1306_setStartLine(0);
#endif  // INCLUDE_SCROLLING
  SSD1306_clear(0, SSD1306_displayWidth() - 1, 0 , SSD1306_displayRows() - 1);
}
//------------------------------------------------------------------------------
void SSD1306_clear(uint8_t c0, uint8_t c1, uint8_t r0, uint8_t r1) {
  // Cancel skip character pixels.
  m_skip = 0;

  // Insure only rows on display will be cleared.
  if (r1 >= SSD1306_displayRows()) r1 = SSD1306_displayRows() - 1;

  for (uint8_t r = r0; r <= r1; r++) {
    SSD1306_setCursor(c0, r);
    for (uint8_t c = c0; c <= c1; c++) {
      // Insure clear() writes zero. result is (m_invertMask^m_invertMask).
      SSD1306_WriteRamBuf(m_invertMask);
    }
  }
  SSD1306_setCursor(c0, r0);
}
//------------------------------------------------------------------------------
void SSD1306_clearToEOL(void) {
  SSD1306_clear(m_col, SSD1306_displayWidth() -1, m_row, m_row + SSD1306_fontRows() - 1);
}
//------------------------------------------------------------------------------
void SSD1306_clearField(uint8_t col, uint8_t row, uint8_t n) {
  SSD1306_clear(col, col + SSD1306_fieldWidth(n) - 1, row, row + SSD1306_fontRows() - 1);
}
//------------------------------------------------------------------------------
void SSD1306_displayRemap(bool mode) {
  SSD1306_WriteCmd(mode ? SSD1306_SEGREMAP : SSD1306_SEGREMAP | 1);
  SSD1306_WriteCmd(mode ? SSD1306_COMSCANINC : SSD1306_COMSCANDEC);
}
//------------------------------------------------------------------------------
size_t SSD1306_fieldWidth(uint8_t n) {
  return n*(SSD1306_fontWidth() + SSD1306_letterSpacing());
}
//------------------------------------------------------------------------------
uint8_t SSD1306_fontCharCount(void) {
  return m_font ? readFontByte(m_font + FONT_CHAR_COUNT) : 0;
}
//------------------------------------------------------------------------------
char SSD1306_fontFirstChar(void) {
  return m_font ? readFontByte(m_font + FONT_FIRST_CHAR) : 0;
}
//------------------------------------------------------------------------------
uint8_t SSD1306_fontHeight(void) {
  return m_font ? m_magFactor*readFontByte(m_font + FONT_HEIGHT) : 0;
}
//------------------------------------------------------------------------------
uint8_t SSD1306_fontRows(void) {
  return m_font ? m_magFactor*((readFontByte(m_font + FONT_HEIGHT) + 7)/8) : 0;
}
//------------------------------------------------------------------------------
uint16_t fontSize(void) {
  return (readFontByte(m_font) << 8) | readFontByte(m_font + 1);
}
//------------------------------------------------------------------------------
uint8_t SSD1306_fontWidth(void) {
  return m_font ? m_magFactor*readFontByte(m_font + FONT_WIDTH) : 0;
}
//------------------------------------------------------------------------------
void SSD1306_init(const DevType* dev) {
  m_col = 0;
  m_row = 0;
#ifdef __AVR__
  const uint8_t* table = (const uint8_t*)pgm_read_word(&dev->initcmds);
#else  // __AVR__
  const uint8_t* table = dev->initcmds;
#endif  // __AVR
  uint8_t size = readFontByte(&dev->initSize);
  m_displayWidth = readFontByte(&dev->lcdWidth);
  m_displayHeight = readFontByte(&dev->lcdHeight);
  m_colOffset = readFontByte(&dev->colOffset);
  for (uint8_t i = 0; i < size; i++) {
    SSD1306_WriteCmd(readFontByte(table + i));
  }
  SSD1306_clearNoArg();
}
//------------------------------------------------------------------------------
void SSD1306_invertDisplay(bool invert) {
  SSD1306_WriteCmd(invert ? SSD1306_INVERTDISPLAY : SSD1306_NORMALDISPLAY);
}
//------------------------------------------------------------------------------
void SSD1306_setCol(uint8_t col) {
  if (col < m_displayWidth) {
    m_col = col;
    col += m_colOffset;
    SSD1306_WriteCmd(SSD1306_SETLOWCOLUMN | (col & 0XF));
    SSD1306_WriteCmd(SSD1306_SETHIGHCOLUMN | (col >> 4));
  }
}
//------------------------------------------------------------------------------
void SSD1306_setContrast(uint8_t value) {
  SSD1306_WriteCmd(SSD1306_SETCONTRAST);
  SSD1306_WriteCmd(value);
}
//------------------------------------------------------------------------------
void SSD1306_setCursor(uint8_t col, uint8_t row) {
  SSD1306_setCol(col);
  SSD1306_setRow(row);
}
//------------------------------------------------------------------------------
void SSD1306_setFont(const uint8_t* font) {
  m_font = font;
  if (font && fontSize() == 1) {
     m_letterSpacing = 0;
  } else {
    m_letterSpacing = 1;
  }
}
//------------------------------------------------------------------------------
void SSD1306_setRow(uint8_t row) {
  if (row < SSD1306_displayRows()) {
    m_row = row;
#if INCLUDE_SCROLLING
    SSD1306_WriteCmd(SSD1306_SETSTARTPAGE | ((m_row + m_pageOffset) & 7));
#else  // INCLUDE_SCROLLING
    SSD1306_WriteCmd(SSD1306_SETSTARTPAGE | m_row);
#endif  // INCLUDE_SCROLLING
  }
}
#if INCLUDE_SCROLLING
//------------------------------------------------------------------------------
void SSD1306_setPageOffset(uint8_t page) {
  m_pageOffset = page & 7;
  SSD1306_setRow(m_row);
}
//------------------------------------------------------------------------------
void SSD1306_setStartLine(uint8_t line) {
  m_startLine = line & 0X3F;
  SSD1306_WriteCmd(SSD1306_SETSTARTLINE | m_startLine);
}
#endif   // INCLUDE_SCROLLING
//------------------------------------------------------------------------------
void SSD1306_WriteRam(uint8_t c) {
  if (m_col < m_displayWidth) {
    writeDisplay(c^m_invertMask, SSD1306_MODE_RAM);
    m_col++;
  }
}
//------------------------------------------------------------------------------
void SSD1306_WriteRamBuf(uint8_t c) {
  if (m_skip) {
    m_skip--;
  } else if (m_col < m_displayWidth) {
    writeDisplay(c^m_invertMask, SSD1306_MODE_RAM_BUF);
    m_col++;
  }
}
//------------------------------------------------------------------------------
GLCDFONTDECL(scaledNibble) = {
  0X00, 0X03, 0X0C, 0X0F,
  0X30, 0X33, 0X3C, 0X3F,
  0XC0, 0XC3, 0XCC, 0XCF,
  0XF0, 0XF3, 0XFC, 0XFF
};
//------------------------------------------------------------------------------
size_t SSD1306_strWidth(const char* str) {
  size_t sw = 0;
  while (*str) {
    uint8_t cw = SSD1306_charWidth(*str++);
    if (cw == 0) {
      return 0;
    }
    sw += cw + SSD1306_letterSpacing();
  }
  return sw;
}
//------------------------------------------------------------------------------
void SSD1306_tickerInit(TickerState* state, const uint8_t* font,
       uint8_t row, bool mag2X, uint8_t bgnCol, uint8_t endCol) {
  state->font = font;
  state->row = row;
  state->mag2X = mag2X;
  state->bgnCol = bgnCol;
  state->endCol = endCol < m_displayWidth ? endCol : m_displayWidth - 1;
  state->nQueue = 0;
}
//------------------------------------------------------------------------------
bool SSD1306_tickerText(TickerState* state, const char* text) {
  if (!text) {
    state->nQueue = 0;
    return true;
  }
  if (state->nQueue >= TICKER_QUEUE_DIM) {
    return false;
  }
  if (state->nQueue == 0) {
    state->init = true;
  }
  state->queue[state->nQueue++] = text;
  return true;
}
//------------------------------------------------------------------------------
int8_t SSD1306_tickerTick(TickerState* state) {
  if (!state->font) {
    return -1;
  }
  if (!state->nQueue) {
    return 0;
  }
  SSD1306_setFont(state->font);
  m_magFactor = state->mag2X ? 2 : 1;
  if (state->init) {
    SSD1306_clear(state->bgnCol, state->endCol, state->row, state->row + SSD1306_fontRows() -1);
    state->col = state->endCol;
    state->skip = 0;
    state->init = false;
  }
  // Adjust display width to truncate pixels after endCol.  Find better way?
  uint8_t save = m_displayWidth;
  m_displayWidth = state->endCol + 1;

  // Skip pixels before bgnCol.
  SSD1306_skipColumns(state->skip);
  SSD1306_setCursor(state->col, state->row);
  for (uint8_t i = 0; i < state->nQueue; i++) {
    const char* str = state->queue[i];
    while (*str && m_col <= state->endCol) {
      SSD1306_write_char(*str++);
    }
    if (m_col > state->endCol) {
      break;
    }
  }
  if (m_col <= state->endCol) {
    SSD1306_clear(m_col, m_col, state->row, state->row + SSD1306_fontRows() - 1);
  }
  // Restore display width.
  m_displayWidth = save;

  if (state->nQueue == 1 && *state->queue[0] == 0) {
    state->nQueue = 0;
    return 0;
  }
  if (state->col > state->bgnCol) {
    state->col--;
  } else {
    state->skip++;
    if (state->skip >= SSD1306_charSpacing(*state->queue[0])) {
      state->skip = 0;
      state->queue[0]++;
      if (*state->queue[0] == 0 && state->nQueue > 1) {
        state->nQueue--;
        for (uint8_t i = 0; i < state->nQueue; i++) {
          state->queue[i] = state->queue[i + 1];
        }
      }
    }
  }
  return state->nQueue;
}
//------------------------------------------------------------------------------
bool SSD1306_write_char(uint8_t ch) {
  if (!m_font) {
    return 0;
  }
  uint8_t w = readFontByte(m_font + FONT_WIDTH);
  uint8_t h = readFontByte(m_font + FONT_HEIGHT);
  uint8_t nr = (h + 7)/8;
  uint8_t first = readFontByte(m_font + FONT_FIRST_CHAR);
  uint8_t count = readFontByte(m_font + FONT_CHAR_COUNT);
  const uint8_t* base = m_font + FONT_WIDTH_TABLE;

  if (ch < first || ch >= (first + count)) {
    if (ch == '\r') {
      SSD1306_setCol(0);
      return 1;
    }
    if (ch == '\n') {
      SSD1306_setCol(0);
      uint8_t fr = m_magFactor*nr;
#if INCLUDE_SCROLLING
      uint8_t dr = SSD1306_displayRows();
      uint8_t tmpRow = m_row + fr;
      int8_t delta = tmpRow + fr - dr;
      if (m_scrollMode == SCROLL_MODE_OFF || delta <= 0) {
        SSD1306_setRow(tmpRow);
      } else {
        m_pageOffset = (m_pageOffset + delta) & 7;
        m_row  = dr - fr;
        // Cursor will be positioned by SSD1306_clearToEOL.
        SSD1306_clearToEOL();
        if (m_scrollMode == SCROLL_MODE_AUTO) {
          SSD1306_setStartLine(8*m_pageOffset);
        }
      }
#else  // INCLUDE_SCROLLING
      SSD1306_setRow(m_row + fr);
#endif  // INCLUDE_SCROLLING
      return 1;
    }
    return 0;
  }
  ch -= first;
  uint8_t s = SSD1306_letterSpacing();
  uint8_t thieleShift = 0;
  if (fontSize() < 2) {
    // Fixed width font.
    base += nr*w*ch;
  } else {
    if (h & 7) {
      thieleShift = 8 - (h & 7);
    }
    uint16_t index = 0;
    for (uint8_t i = 0; i < ch; i++) {
      index += readFontByte(base + i);
    }
    w = readFontByte(base + ch);
    base += nr*index + count;
  }
  uint8_t scol = m_col;
  uint8_t srow = m_row;
  uint8_t skip = m_skip;
  for (uint8_t r = 0; r < nr; r++) {
    for (uint8_t m = 0; m < m_magFactor; m++) {
      SSD1306_skipColumns(skip);
      if (r || m) {
        SSD1306_setCursor(scol, m_row + 1);
      }
      for (uint8_t c = 0; c < w; c++) {
        uint8_t b = readFontByte(base + c + r*w);
        if (thieleShift && (r + 1) == nr) {
          b >>= thieleShift;
        }
        if (m_magFactor == 2) {
           b = m ?  b >> 4 : b & 0XF;
           b = readFontByte(scaledNibble + b);
           SSD1306_WriteRamBuf(b);
        }
        SSD1306_WriteRamBuf(b);
      }
      for (uint8_t i = 0; i < s; i++) {
        SSD1306_WriteRamBuf(0);
      }
    }
  }
  SSD1306_setRow(srow);
  return 1;
}

void SSD1306_oledPower(bool on)
{
	if (on) {
		SSD1306_WriteCmd(0xaf); // turn on OLED
	}
	else {
		SSD1306_WriteCmd(0xae); // turn off OLED
	}
}

uint8_t SSD1306_pageOffset(void) {return m_pageOffset;}
uint8_t SSD1306_pageOffsetLine(void) {return 8*m_pageOffset;}
void SSD1306_scrollDisplay(int8_t lines) {SSD1306_setStartLine(m_startLine + lines);}
void SSD1306_scrollMemory(int8_t rows) {SSD1306_setPageOffset(m_pageOffset + rows);}
bool SSD1306_scrollIsSynced(void) {return SSD1306_startLine() == SSD1306_pageOffsetLine();}
void SSD1306_setScrollMode(uint8_t mode) {m_scrollMode = mode;}
uint8_t SSD1306_startLine(void) {return m_startLine;}
uint8_t SSD1306_charSpacing(uint8_t c) {return SSD1306_charWidth(c) + SSD1306_letterSpacing();}
uint8_t SSD1306_col(void) {return m_col;}
uint8_t SSD1306_displayHeight(void) {return m_displayHeight;}
uint8_t SSD1306_displayRows(void) {return m_displayHeight/8;}
uint8_t SSD1306_displayWidth(void) {return m_displayWidth;}
const uint8_t* SSD1306_font(void) {return m_font;}
void SSD1306_home(void) {SSD1306_setCursor(0, 0);}
bool SSD1306_invertMode(void) {return !!m_invertMask;}
void SSD1306_setInvertMode(bool mode) {m_invertMask = mode ? 0XFF : 0;}
uint8_t SSD1306_letterSpacing(void) {return m_magFactor*m_letterSpacing;}
uint8_t SSD1306_magFactor(void) {return m_magFactor;}
uint8_t SSD1306_row(void) {return m_row;}
void SSD1306_set1X(void) {m_magFactor = 1;}
void SSD1306_set2X(void) {m_magFactor = 2;}
void SSD1306_setLetterSpacing(uint8_t pixels) {m_letterSpacing = pixels;}
void SSD1306_WriteCmd(uint8_t c) {writeDisplay(c, SSD1306_MODE_CMD);}
void SSD1306_skipColumns(uint8_t n) {m_skip = n;}
// bool SSD1306_tickerText(TickerState* state, const String &str) {
// 	return SSD1306_tickerText(state, str.c_str());
// }

bool SSD1306_write_str(char* str)
{
	bool result = 0;
	for(int i = 0; i < strlen(str); i++) result |= SSD1306_write_char(str[i]);
	return result;
}

bool SSD1306_writeln_str(char* str)
{
	uint8_t len = strlen(str);
	char newln_str[len + 2];
	memset(newln_str, 0, len+2);
	strlcpy(newln_str, str, len);
	newln_str[len] = '\n';
	return SSD1306_write_str(newln_str);
}

void writeDisplay(uint8_t b, uint8_t mode) {
	#if OPTIMIZE_I2C
	if (m_nData > 16 || (m_nData && mode == SSD1306_MODE_CMD)) {
		//m_oledWire.endTransmission();
		i2c_master_send_stop(&i2c_master_instance);
		m_nData = 0;
	}
	if (m_nData == 0) {
		//m_oledWire.beginTransmission(m_i2cAddr);
		struct i2c_master_packet packet = {
			.address = screen_address,
			.data = 0,
			.data_length = 0,
			.ten_bit_address = false,
			.high_speed      = false,
			.hs_master_code  = 0x0,
		};
		i2c_master_write_packet_wait_no_stop(&i2c_master_instance, &packet);
		_I2CWrite(mode == SSD1306_MODE_CMD ? 0X00 : 0X40);
	}
	_I2CWrite(b);
	if (mode == SSD1306_MODE_RAM_BUF) {
		m_nData++;
	}
	else {
		//m_oledWire.endTransmission();
		i2c_master_send_stop(&i2c_master_instance);
		m_nData = 0;
	}
// 	#else  // OPTIMIZE_I2C
// 	m_oledWire.beginTransmission(m_i2cAddr);
// 	m_oledWire.write(mode == SSD1306_MODE_CMD ? 0X00: 0X40);
// 	m_oledWire.write(b);
// 	m_oledWire.endTransmission();
 	#endif    // OPTIMIZE_I2C
}

static void _I2CWrite(uint8_t byte)
{
	i2c_master_write_byte(&i2c_master_instance, byte);
}
