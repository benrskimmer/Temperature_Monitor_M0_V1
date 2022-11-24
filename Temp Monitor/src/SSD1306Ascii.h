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
/**
* @file SSD1306Ascii.h
* @brief Base class for ssd1306 displays.
*/
#ifndef SSD1306Ascii_h
#define SSD1306Ascii_h
//#include "Arduino.h"
#include "SSD1306init.h"
#include "fonts/allFonts.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <stdbool.h>
//------------------------------------------------------------------------------
/** SSD1306Ascii version */
#define SDD1306_ASCII_VERSION 10302
//------------------------------------------------------------------------------
// Configuration options.
/** Set Scrolling mode for newline.
*
* If INCLUDE_SCROLLING is defined to be zero, newline will not scroll
* the display and code for scrolling will not be included.  This option
* will save some code space and three bytes of RAM.
*
* If INCLUDE_SCROLLING is nonzero, the scroll feature will included.
*/
#define INCLUDE_SCROLLING 1

/** Initial scroll mode, SCROLL_MODE_OFF,
SCROLL_MODE_AUTO, or SCROLL_MODE_APP. */
#define INITIAL_SCROLL_MODE SCROLL_MODE_OFF

/** Dimension of TickerState pointer queue */
#define TICKER_QUEUE_DIM 6

/** Use larger faster I2C code. */
#define OPTIMIZE_I2C 1

/** If MULTIPLE_I2C_PORTS is nonzero,
define a constructor with port selection. */
#ifdef __AVR__
// Save memory on AVR. Set nonzero to use alternate I2C or software I2c on AVR.
#define MULTIPLE_I2C_PORTS 0
#else  // __AVR__
#define MULTIPLE_I2C_PORTS 1
#endif  // __AVR__

/** AvrI2c uses 400 kHz fast mode if AVRI2C_FASTMODE is nonzero else 100 kHz. */
//#define AVRI2C_FASTMODE 1
//------------------------------------------------------------------------------
// Values for setScrolMode(uint8_t mode)
/** Newline will not scroll the display or RAM window. */
#define SCROLL_MODE_OFF  0
/** Newline will scroll both the display and RAM windows. */
#define SCROLL_MODE_AUTO 1
/** Newline scrolls the RAM window. The app scrolls the display window. */
#define SCROLL_MODE_APP  2
//------------------------------------------------------------------------------
// Values for writeDisplay() mode parameter.
/** Write to Command register. */
#define SSD1306_MODE_CMD     0
/** Write one byte to display RAM. */
#define SSD1306_MODE_RAM     1
/** Write to display RAM with possible buffering. */
#define SSD1306_MODE_RAM_BUF 2
//------------------------------------------------------------------------------
/**
* @brief Reset the display controller.
*
* @param[in] rst Reset pin number.
*/
// inline void oledReset(uint8_t rst) {
//   pinMode(rst, OUTPUT);
//   digitalWrite(rst, LOW);
//   delay(10);
//   digitalWrite(rst, HIGH);
//   delay(10);
// }
//------------------------------------------------------------------------------
/**
* @struct TickerState
* @brief ticker state
*/
typedef struct TickerStateStruct {
const char* queue[TICKER_QUEUE_DIM];  ///< Queue of text pointers.
uint8_t nQueue;// = 0;  ///< Count of pointers in queue.
const uint8_t* font;// = nullptr;  ///< Font for ticker.
bool mag2X;      ///< Use mag2X if true.
uint8_t row;     ///< Row for ticker
uint8_t bgnCol;  ///< Begin column of ticker.
uint8_t endCol;  ///< End column of ticker.
bool init;       ///< clear and initialize display area if true.
uint8_t col;     ///< Column for start of displayed text.
uint8_t skip;    ///< Number of pixels to skip in first character.
/// @return Count of free queue slots.
//uint8_t queueFree(void) {return TICKER_QUEUE_DIM - nQueue;}
/// @return Count of used queue slots.
//uint8_t queueUsed(void) {return nQueue;}
}TickerState;
//------------------------------------------------------------------------------
/**
* @class SSD1306Ascii
* @brief SSD1306 base class
*/
// class SSD1306Ascii : public Print {
//  public:
//   using Print::write;
//   SSD1306Ascii() {}
#if INCLUDE_SCROLLING
//------------------------------------------------------------------------------

/**
* Start function that initializes the driver library
*/
void SSD1306_begin(uint8_t i2cAddr);

int oledInit(const DevType* dev);

void SSD1306_oledPower(bool on);

/**
* @return the RAM page for top of the RAM window.
*/
uint8_t SSD1306_pageOffset(void);
/**
* @return the display line for pageOffset.
*/
uint8_t SSD1306_pageOffsetLine(void);
/**
* @brief Scroll the Display window.
*
* @param[in] lines Number of lines to scroll the window.
*/
void SSD1306_scrollDisplay(int8_t lines);
/**
* @brief Scroll the RAM window.
*
* @param[in] rows Number of rows to scroll the window.
*/
void SSD1306_scrollMemory(int8_t rows);
/**
* @return true if the first display line is equal to the
*         start of the RAM window.
*/
bool SSD1306_scrollIsSynced(void);
/**
* @brief Set page offset.
*
* @param[in] page the RAM page for start of the RAM window
*/
void SSD1306_setPageOffset(uint8_t page);
/**
* @brief Enable or disable scroll mode. Deprecated use setScrollMode().
*
* @param[in] enable true enable scroll on newline false disable scroll.
*/
//   void SSD1306_setScroll(bool enable) __attribute__((deprecated("use setScrollMode"))) {
//     SSD1306_setScrollMode(enable ? SCROLL_MODE_AUTO : SCROLL_MODE_OFF);
//   }
/**
* @brief Set scroll mode.
*
* @param[in] mode One of the following.
*
* SCROLL_MODE_OFF - newline will not scroll the display or RAM window.
*
* SCROLL_MODE_AUTO - newline will scroll both the display and RAM windows.
*
* SCROLL_MODE_APP - newline scrolls the RAM window.
*                   The app scrolls the display window.
*/
void SSD1306_setScrollMode(uint8_t mode);
/**
* @brief Set the display start line register.
*
* @param[in] line RAM line to be mapped to first display line.
*/
void SSD1306_setStartLine(uint8_t line);
/**
* @return the display startline.
*/
uint8_t SSD1306_startLine(void);
#endif  // INCLUDE_SCROLLING
//----------------------------------------------------------------------------
/**
* @brief Determine the spacing of a character. Spacing is width + space.
*
* @param[in] c Character code.
* @return Spacing of the character in pixels.
*/
uint8_t SSD1306_charSpacing(uint8_t c);
/**
* @brief Determine the width of a character.
*
* @param[in] c Character code.
* @return Width of the character in pixels.
*/
uint8_t SSD1306_charWidth(uint8_t c);
/**
* @brief Clear the display and set the cursor to (0, 0).
*/
void SSD1306_clearNoArg(void);
/**
* @brief Clear a region of the display.
*
* @param[in] c0 Starting column.
* @param[in] c1 Ending column.
* @param[in] r0 Starting row;
* @param[in] r1 Ending row;
* @note The final cursor position will be (c0, r0).
*/
void SSD1306_clear(uint8_t c0, uint8_t c1, uint8_t r0, uint8_t r1);
/**
* @brief Clear a field of n fieldWidth() characters.
*
* @param[in] col Field start column.
*
* @param[in] row Field start row.
*
* @param[in] n Number of characters in the field.
*
*/
void SSD1306_clearField(uint8_t col, uint8_t row, uint8_t n);
/**
* @brief Clear the display to the end of the current line.
* @note The number of rows cleared will be determined by the height
*       of the current font.
* @note The cursor will be returned to the original position.
*/
void SSD1306_clearToEOL(void);
/**
* @return The current column in pixels.
*/
uint8_t SSD1306_col(void);
/**
* @return The display hight in pixels.
*/
uint8_t SSD1306_displayHeight(void);
/**
* @brief Set display to normal or 180 degree remap mode.
*
* @param[in] mode true for normal mode, false for remap mode.
*
* @note Adafruit and many ebay displays use remap mode.
*       Use normal mode to rotate these displays 180 degrees.
*/
void SSD1306_displayRemap(bool mode);
/**
* @return The display height in rows with eight pixels to a row.
*/
uint8_t SSD1306_displayRows(void);
/**
* @return The display width in pixels.
*/
uint8_t SSD1306_displayWidth(void);
/**
* @brief Width of a field in pixels.
*
* @param[in] n Number of characters in the field.
*
* @return Width of the field.
*/
size_t SSD1306_fieldWidth(uint8_t n);
/**
* @return The current font pointer.
*/
const uint8_t* SSD1306_font(void);
/**
* @return The count of characters in a font.
*/
uint8_t SSD1306_fontCharCount(void);
/**
* @return The first character in a font.
*/
char SSD1306_fontFirstChar(void);
/**
* @return The current font height in pixels.
*/
uint8_t SSD1306_fontHeight(void);
/**
* @return The number of eight pixel rows required to display a character
*    in the current font.
*/
uint8_t SSD1306_fontRows(void);
/**
* @return The maximum width of characters in the current font.
*/
uint8_t SSD1306_fontWidth(void);
/**
* @brief Set the cursor position to (0, 0).
*/
void SSD1306_home(void);
/**
* @brief Initialize the display controller.
*
* @param[in] dev A display initialization structure.
*/
void SSD1306_init(const DevType* dev);
/**
* @brief Set pixel mode for for entire display.
*
* @param[in] invert Inverted display if true or normal display if false.
*/
void SSD1306_invertDisplay(bool invert);
/**
* @return invert mode.
*/
bool SSD1306_invertMode(void);
/**
* @brief Set invert mode for write/print.
*
* @param[in] mode Invert pixels if true and use normal mode if false.
*/
void SSD1306_setInvertMode(bool mode);
/**
* @return letter-spacing in pixels with magnification factor.
*/
uint8_t SSD1306_letterSpacing(void);
/**
* @return The character magnification factor.
*/
uint8_t SSD1306_magFactor(void);
/**
* @return the current row number with eight pixels to a row.
*/
uint8_t SSD1306_row(void);
/**
* @brief Set the character magnification factor to one.
*/
void SSD1306_set1X(void);
/**
* @brief Set the character magnification factor to two.
*/
void SSD1306_set2X(void);
/**
* @brief Set the current column number.
*
* @param[in] col The desired column number in pixels.
*/
void SSD1306_setCol(uint8_t col);
/**
* @brief Set the display contrast.
*
* @param[in] value The contrast level in th range 0 to 255.
*/
void SSD1306_setContrast(uint8_t value);
/**
* @brief Set the cursor position.
*
* @param[in] col The column number in pixels.
* @param[in] row the row number in eight pixel rows.
*/
void SSD1306_setCursor(uint8_t col, uint8_t row);
/**
* @brief Set the current font.
*
* @param[in] font Pointer to a font table.
*/
void SSD1306_setFont(const uint8_t* font);
/**
* @brief Set letter-spacing.  setFont() will restore default letter-spacing.
*
* @param[in] pixels letter-spacing in pixels before magnification.
*/
void SSD1306_setLetterSpacing(uint8_t pixels);
/**
* @brief Set the current row number.
*
* @param[in] row the row number in eight pixel rows.
*/
void SSD1306_setRow(uint8_t row);
/**
* @brief Write a command byte to the display controller.
*
* @param[in] c The command byte.
* @note The byte will immediately be sent to the controller.
*/
void SSD1306_WriteCmd(uint8_t c);
/**
* @brief Write a byte to RAM in the display controller.
*
* @param[in] c The data byte.
* @note The byte will immediately be sent to the controller.
*/
void SSD1306_WriteRam(uint8_t c);
/**
* @brief Write a byte to RAM in the display controller.
*
* @param[in] c The data byte.
* @note The byte may be buffered until a call to ssd1306WriteCmd
*       or ssd1306WriteRam.
*/
void SSD1306_WriteRamBuf(uint8_t c);
/**
* @brief Skip leading pixels writing characters to display display RAM.
*
* @param[in] n Number of pixels to skip.
*/
void SSD1306_skipColumns(uint8_t n);
/**
* @brief Character width.
*
* @param[in] str The pointer to string.
* @return the width of the string in pixels.
*/
size_t SSD1306_strWidth(const char* str);
/**
* @brief Initialize TickerState struct.
*
* @param[in,out] state Ticker state.
* @param[in] font Font to be displayed.
* @param[in] row Row for ticker.
* @param[in] mag2X Set magFactor to two if true.
* @param[in] bgnCol First column of ticker. Default is zero.
* @param[in] endCol Last column of ticker. Default is last column of display.
*/
void SSD1306_tickerInit(TickerState* state, const uint8_t* font, uint8_t row,
    bool mag2X, uint8_t bgnCol, uint8_t endCol);
// void SSD1306_tickerInit(TickerState* state, const uint8_t* font, uint8_t row,
// bool mag2X = false, uint8_t bgnCol = 0, uint8_t endCol = 255);
/**
*  @brief Add text pointer to display queue.
*
* @param[in,out] state Ticker state.
* @param[in] str Pointer to String object. Clear queue if nullptr.
* @return false if queue is full else true.
*/
//bool SSD1306_tickerText(TickerState* state, const String &str);
/**
*  @brief Add text pointer to display queue.
*
* @param[in,out] state Ticker state.
* @param[in] text Pointer to C string.  Clear queue if nullptr.
* @return false if queue is full else true.
*/
bool SSD1306_tickerText(TickerState* state, const char* text);
/**
* @brief Advance ticker by one pixel.
*
* @param[in,out] state Ticker state.
* @return Number of entries in text pointer queue.
*/
int8_t SSD1306_tickerTick(TickerState* state);
/**
* @brief Display a character.
*
* @param[in] c The character to display.
* @return one for success else zero.
*/
bool SSD1306_write_char(uint8_t c);
/**
* @brief Display a null terminated string
*
* @param[in] pointer to a char string - must be null terminated
* @return one for success else zero.
*/
bool SSD1306_write_str(char* str);
/**
* @brief Display a null terminated string and add new line
*
* @param[in] pointer to a char string - must be null terminated
* @return one for success else zero.
*/
bool SSD1306_writeln_str(char* str);


#endif  // SSD1306Ascii_h
