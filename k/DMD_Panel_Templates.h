#pragma once
#include "DMD_RGB.h"

#ifndef DMD_PANEL_TEMPLATES_H
#define DMD_PANEL_TEMPLATES_H
/*--------------------------------------------------------------------------------------
 This file is a part of the DMD_STM32 library

 DMD_STM32.h  - STM32 port of DMD.h library

 https://github.com/board707/DMD_STM32
 Dmitry Dmitriev (c) 2019-2023
 /--------------------------------------------------------------------------------------*/

// Pattern types for panels with multiplex = 2

#define PATTERN_UPS   0b0111
#define PATTERN_UPD   0b0110
#define PATTERN_UNS   0b0101
#define PATTERN_UND   0b0100
#define PATTERN_LPS   0b0011
#define PATTERN_LPD   0b0010
#define PATTERN_LNS   0b0001
#define PATTERN_LND   0b0000

// == MATRIX TYPES ==

/*--------------------------------------------------------------------------------------*/
//   == Syntax of panel define ==
//
//   #define NAME_OF_PANEL  MUX pins, Panel_width, Panel_height, scan, pattern_ID
//
// example:
//   #define RGB32x32_S8_maxmurugan  3,32,32,8,3	// 3 mux pins(A B C), 32x32, scan 1/8, pattern 3
//
//             pattern_ID is unique number for pattern design
//
//   == Variadic template patterns == (since v1.1.5)
//
//   #define NAME_OF_PANEL  MUX pins, Panel_width, Panel_height, scan, pattern_ID, Pixbase, Pattern_type
//
//            Variadic patterns:  101 - similar to patter pt1, siplicified, LPS/UPS type only
//								  102 - similar to patter pt2, siplicified, LPS/UPS type only
//								  111 - similar to patter pt1, generalized, any type
//								  112 - similar to patter pt2, generalized, any type
/*--------------------------------------------------------------------------------------*/

// P6 32x32 1/8 outdoor, BINARY mux
#define RGB32x32_S8_maxmurugan		3,32,32,8,3	// 32x32 1/8 BINARY from @maxmurugan

// 32x32 plain, single row scan (scan = height / 2)
#define RGB32x32plainS16  4,32,32,16,0		// 32x32 1/16

// 32x32 1/8 plain pattern, BINARY mux - for dmd_rgb_pattern_test only
#define RGB32x32plainS8   3,32,32,8,1		// 32x32 1/8 pattern 1

/*--------------------------------------------------------------------------------------*/
template <int MUX_CNT, int P_Width, int P_Height, int SCAN, int SCAN_TYPE, int... Pars>

class DMD_RGB : public DMD_RGB_BASE2<Pars...>
{
public:
	DMD_RGB() = 0;

};

//--------------------------------------------------------------------------------------/
// Matrix types templates
//--------------------------------------------------------------------------------------/
// Plain panels with  Scan = P_Height / 2
//--------------------------------------------------------------------------------------/

template <int MUX_CNT, int P_Width, int P_Height, int SCAN, int COL_DEPTH>
class DMD_RGB<MUX_CNT, P_Width, P_Height, SCAN, 0, COL_DEPTH> : public DMD_RGB_BASE2<COL_DEPTH>
{
public:
	DMD_RGB(uint8_t* mux_list, byte _pin_nOE, byte _pin_SCLK, uint8_t* pinlist,
		byte panelsWide, byte panelsHigh, bool d_buf = false) :
		DMD_RGB_BASE2<COL_DEPTH>(MUX_CNT, mux_list, _pin_nOE, _pin_SCLK, pinlist,
			panelsWide, panelsHigh, d_buf, COL_DEPTH, SCAN, P_Width, P_Height)
	{}
};

//--------------------------------------------------------------------------------------
// "plain" type outdoor matrix, BINARY/DIRECT mux
// with consecutive bytes in consecutive horizontal lines
// (0,1) (2,3)...
// left for testing
//--------------------------------------------------------------------------------------/
template <int MUX_CNT, int P_Width, int P_Height, int SCAN, int COL_DEPTH>
class DMD_RGB<MUX_CNT, P_Width, P_Height, SCAN, 1, COL_DEPTH> : public DMD_RGB_BASE2<COL_DEPTH>
{
public:
	DMD_RGB(uint8_t* mux_list, byte _pin_nOE, byte _pin_SCLK, uint8_t* pinlist,
		byte panelsWide, byte panelsHigh, bool d_buf = false) :
		DMD_RGB_BASE2<COL_DEPTH>(MUX_CNT, mux_list, _pin_nOE, _pin_SCLK, pinlist,
			panelsWide, panelsHigh, d_buf, COL_DEPTH, SCAN, P_Width, P_Height)
	{}

protected:
	uint16_t get_base_addr(int16_t& x, int16_t& y) override {
		this->transform_XY(x, y);
		uint8_t pol_y = y % this->pol_displ;
		x += (y / this->DMD_PIXELS_DOWN) * this->WIDTH;
		uint16_t base_addr = (pol_y / this->multiplex) * this->x_len +
			(x / this->DMD_PIXELS_ACROSS) * this->multiplex * this->DMD_PIXELS_ACROSS +
			(pol_y % this->multiplex) * this->DMD_PIXELS_ACROSS + x % this->DMD_PIXELS_ACROSS;
		return base_addr;
	}
};


//--------------------------------------------------------------------------------------
// "plain" type outdoor matrix, BINARY/DIRECT mux
// with consecutive bytes in horizontal lines
// separated by nRows lines
// (0,nRows) (1, nRows+1)...
// left for testing
//--------------------------------------------------------------------------------------/
template <int MUX_CNT, int P_Width, int P_Height, int SCAN, int COL_DEPTH>
class DMD_RGB<MUX_CNT, P_Width, P_Height, SCAN, 2, COL_DEPTH> : public DMD_RGB_BASE2<COL_DEPTH>
{
public:
	DMD_RGB(uint8_t* mux_list, byte _pin_nOE, byte _pin_SCLK, uint8_t* pinlist,
		byte panelsWide, byte panelsHigh, bool d_buf = false) :
		DMD_RGB_BASE2<COL_DEPTH>(MUX_CNT, mux_list, _pin_nOE, _pin_SCLK, pinlist,
			panelsWide, panelsHigh, d_buf, COL_DEPTH, SCAN, P_Width, P_Height)
	{}

protected:
	uint16_t get_base_addr(int16_t& x, int16_t& y) override {
		this->transform_XY(x, y);
		uint8_t pol_y = y % this->pol_displ;
		x += (y / this->DMD_PIXELS_DOWN) * this->WIDTH;
		uint16_t base_addr = (pol_y % this->nRows) * this->x_len +
			(x / this->DMD_PIXELS_ACROSS) * this->multiplex * this->DMD_PIXELS_ACROSS +
			(pol_y / this->nRows) * this->DMD_PIXELS_ACROSS + x % this->DMD_PIXELS_ACROSS;
		return base_addr;
	}
};

//--------------------------------------------------------------------------------------
// "plain" type outdoor matrix, BINARY/DIRECT mux
// with consecutive bytes in horizontal lines
// separated by nRows lines
// scan quarter of height (i.e 32x32 1/8), start scan from nRows line
// (nRows, 0) (nRows+1, 1)...
//
// p6 32x32 scan 8 matrix from @maxmurugan
//--------------------------------------------------------------------------------------/
template <int MUX_CNT, int P_Width, int P_Height, int SCAN, int COL_DEPTH>
class DMD_RGB<MUX_CNT, P_Width, P_Height, SCAN, 3, COL_DEPTH> : public DMD_RGB_BASE2<COL_DEPTH>
{
public:
	DMD_RGB(uint8_t* mux_list, byte _pin_nOE, byte _pin_SCLK, uint8_t* pinlist,
		byte panelsWide, byte panelsHigh, bool d_buf = false) :
		DMD_RGB_BASE2<COL_DEPTH>(MUX_CNT, mux_list, _pin_nOE, _pin_SCLK, pinlist,
			panelsWide, panelsHigh, d_buf, COL_DEPTH, SCAN, P_Width, P_Height)
	{
        this->fast_Hbyte = false;
        this->use_shift = false;
        }
    // Fast text shift is disabled for complex patterns, so we don't need the method
        void disableFastTextShift(bool shift) override {UNUSED1(shift);}

protected:
	uint16_t get_base_addr(int16_t& x, int16_t& y) override {
		this->transform_XY(x, y);
		uint8_t pol_y = y % this->pol_displ;
		x += (y / this->DMD_PIXELS_DOWN) * this->WIDTH;
		uint16_t base_addr = (pol_y % this->nRows) * this->x_len +
            (x / this->DMD_PIXELS_ACROSS) * this->multiplex * this->DMD_PIXELS_ACROSS +
            x % this->DMD_PIXELS_ACROSS;
            if (pol_y < this->nRows)  base_addr += this->DMD_PIXELS_ACROSS;
		return base_addr;
	}
};

//--------------------------------------------------------------------------------------
//  *** Variadic templates ***
// outdoor matrix, BINARY/DIRECT mux
// with parametrized Pixbase consecutive bytes
// 101 is analog to pattern 1 above, 102 is analog to 2
// scan quarter of height (i.e 64x32 1/8),
// The second parameter Pattern - will set UPS/LPS type  UPS = 7, LPS =3
//--------------------------------------------------------------------------------------/
template <int MUX_CNT, int P_Width, int P_Height, int SCAN, int Pixbase, int Pattern, int COL_DEPTH>
class DMD_RGB<MUX_CNT, P_Width, P_Height, SCAN, 101, Pixbase, Pattern, COL_DEPTH> : public DMD_RGB_BASE2<COL_DEPTH>
{
public:
	DMD_RGB(uint8_t* mux_list, byte _pin_nOE, byte _pin_SCLK, uint8_t* pinlist,
		byte panelsWide, byte panelsHigh, bool d_buf = false) :
		DMD_RGB_BASE2<COL_DEPTH>(MUX_CNT, mux_list, _pin_nOE, _pin_SCLK, pinlist,
			panelsWide, panelsHigh, d_buf, COL_DEPTH, SCAN, P_Width, P_Height)
	{
        this->fast_Hbyte = false;
        this->use_shift = false;
        }
    // Fast text shift is disabled for complex patterns, so we don't need the method
		void disableFastTextShift(bool shift) override {UNUSED1(shift);}

protected:

	uint16_t get_base_addr(int16_t& x, int16_t& y) override {
		this->transform_XY(x, y);
		uint8_t pol_y = y % this->pol_displ;
		x += (y / this->DMD_PIXELS_DOWN) * this->WIDTH;
		uint16_t base_addr = (pol_y / this->multiplex) * this->x_len +
            (x / Pixbase) * this->multiplex * Pixbase + x % Pixbase;
            if (Pattern == 3) { if ( ! (pol_y % this->multiplex)) base_addr += Pixbase; }      // LPS
			else if (Pattern == 7) { if (pol_y % this->multiplex)  base_addr += Pixbase; }     // UPS
			else { base_addr += (pol_y % this->multiplex) * Pixbase; }
		return base_addr;
	}

};

template <int MUX_CNT, int P_Width, int P_Height, int SCAN, int Pixbase, int Pattern, int COL_DEPTH>
class DMD_RGB<MUX_CNT, P_Width, P_Height, SCAN, 102, Pixbase, Pattern, COL_DEPTH> : public DMD_RGB_BASE2<COL_DEPTH>
{
public:
	DMD_RGB(uint8_t* mux_list, byte _pin_nOE, byte _pin_SCLK, uint8_t* pinlist,
		byte panelsWide, byte panelsHigh, bool d_buf = false) :
		DMD_RGB_BASE2<COL_DEPTH>(MUX_CNT, mux_list, _pin_nOE, _pin_SCLK, pinlist,
			panelsWide, panelsHigh, d_buf, COL_DEPTH, SCAN, P_Width, P_Height)
	{
        this->fast_Hbyte = false;
        this->use_shift = false;
        }
    // Fast text shift is disabled for complex patterns, so we don't need the method
        void disableFastTextShift(bool shift) override {UNUSED1(shift);}

protected:

	uint16_t get_base_addr(int16_t& x, int16_t& y) override {
		this->transform_XY(x, y);
		uint8_t pol_y = y % this->pol_displ;
		x += (y / this->DMD_PIXELS_DOWN) * this->WIDTH;
		uint16_t base_addr = (pol_y % this->nRows) * this->x_len +
            (x / Pixbase) * this->multiplex * Pixbase + x % Pixbase;
            if (Pattern == 3) { if (pol_y < this->nRows)  base_addr += Pixbase; }          // LPS
			else if (Pattern == 7) { if (pol_y >= this->nRows)  base_addr += Pixbase; }    // UPS
			else { base_addr += (pol_y / this->nRows) * Pixbase; }
		return base_addr;
	}

};

//--------------------------------------------------------------------------------------
//  *** Variadic templates ***
// outdoor matrix, BINARY/DIRECT mux
// with parametrized Pixbase consecutive byte, generalized
// 111 is analog to pattern 1 above, 112 is analog to 2
// scan quarter of height (i.e 64x32 1/8),
// The second parameter Pattern constructed with three bits (high to low)
//   U/L - ether the first oktet (8 pixs) is lighten Upper than second or vise versa
//   P/N - ether the first octet goes to Positive X direction ( left to right) or Negative
//   S/D - ether the second octet goes to the Same direction as first or not
//--------------------------------------------------------------------------------------/

//--------------------------------------------------------------------------------------/
template <int MUX_CNT, int P_Width, int P_Height, int SCAN, int Pixbase, int Pattern, int COL_DEPTH>
class DMD_RGB<MUX_CNT, P_Width, P_Height, SCAN, 111, Pixbase, Pattern, COL_DEPTH> : public DMD_RGB_BASE2<COL_DEPTH>
{
public:
	DMD_RGB(uint8_t* mux_list, byte _pin_nOE, byte _pin_SCLK, uint8_t* pinlist,
		byte panelsWide, byte panelsHigh, bool d_buf = false) :
		DMD_RGB_BASE2<COL_DEPTH>(MUX_CNT, mux_list, _pin_nOE, _pin_SCLK, pinlist,
			panelsWide, panelsHigh, d_buf, COL_DEPTH, SCAN, P_Width, P_Height)
	{
        this->fast_Hbyte = false;
        this->use_shift = false;
        }
    // Fast text shift is disabled for complex patterns, so we don't need the method
        void disableFastTextShift(bool shift) override {UNUSED1(shift);}

protected:

	uint16_t get_base_addr(int16_t& x, int16_t& y) override {
		this->transform_XY(x, y);
		uint8_t pol_y = y % this->pol_displ;
		x += (y / this->DMD_PIXELS_DOWN) * this->WIDTH;

		constexpr bool Upper = (Pattern & 0xf) & 0b0100;
        constexpr bool Pos =   (Pattern & 0xf) & 0b0010;
        constexpr bool Same =  (Pattern & 0xf) & 0b0001;

		uint16_t base_addr = (pol_y / this->multiplex) * this->x_len +
            (x / Pixbase) * this->multiplex * Pixbase;

		bool B = (( ! (pol_y % this->multiplex))^ Upper);  // number of current line in multiplex, 0/1

      if (B) // B=1  (second line)
      {
        base_addr += Pixbase;
		if (!(Pos ^ Same)) base_addr += x % Pixbase;
        else base_addr += (Pixbase - x % Pixbase -1);

      }
      else   // first line
      {
        if (Pos) base_addr += x % Pixbase;
        else base_addr += (Pixbase - x % Pixbase -1);
      }

		return base_addr;
	}

};

template <int MUX_CNT, int P_Width, int P_Height, int SCAN, int Pixbase, int Pattern, int COL_DEPTH>
class DMD_RGB<MUX_CNT, P_Width, P_Height, SCAN, 112, Pixbase, Pattern, COL_DEPTH> : public DMD_RGB_BASE2<COL_DEPTH>
{
public:
	DMD_RGB(uint8_t* mux_list, byte _pin_nOE, byte _pin_SCLK, uint8_t* pinlist,
		byte panelsWide, byte panelsHigh, bool d_buf = false) :
		DMD_RGB_BASE2<COL_DEPTH>(MUX_CNT, mux_list, _pin_nOE, _pin_SCLK, pinlist,
			panelsWide, panelsHigh, d_buf, COL_DEPTH, SCAN, P_Width, P_Height)
	{
        this->fast_Hbyte = false;
        this->use_shift = false;
        }
    // Fast text shift is disabled for complex patterns, so we don't need the method
        void disableFastTextShift(bool shift) override {UNUSED1(shift);}

protected:

	uint16_t get_base_addr(int16_t& x, int16_t& y) override {
		this->transform_XY(x, y);
		uint8_t pol_y = y % this->pol_displ;
		x += (y / this->DMD_PIXELS_DOWN) * this->WIDTH;

		constexpr bool Upper = (Pattern & 0xf) & 0b0100;
        constexpr bool Pos =   (Pattern & 0xf) & 0b0010;
        constexpr bool Same =  (Pattern & 0xf) & 0b0001;

		uint16_t base_addr = (pol_y % this->nRows) * this->x_len +
            (x / Pixbase) * this->multiplex * Pixbase;

			 bool B = (((pol_y < this->nRows) ^ Upper));
       if (B) // B=1  (second line)
      {
        base_addr += Pixbase;
		if (!(Pos ^ Same)) base_addr += x % Pixbase;
        else base_addr += (Pixbase - x % Pixbase -1);

      }
      else   // first line
      {
        if (Pos) base_addr += x % Pixbase;
        else base_addr += (Pixbase - x % Pixbase -1);
      }

		return base_addr;
	}

};
//--------------------------------------------------------------------------------------
//  *** Variadic templates ***
// outdoor matrix, BINARY/DIRECT mux
// with parametrized Pixbase consecutive byte, generalized
// 121 is analog to pattern 1 above, 122 is analog to 2
// scan quarter of height (i.e 64x32 1/8),
//
//   Pattern with LPS/UPS switched every pixbase pixels
//   Pattern = 0 - start from upper line
//   Pattern = 1 - start from lower line
//
// used for @digi55 80x40 s10 panel issue 121
//--------------------------------------------------------------------------------------/

template <int MUX_CNT, int P_Width, int P_Height, int SCAN, int Pixbase, int Pattern, int COL_DEPTH>
class DMD_RGB<MUX_CNT, P_Width, P_Height, SCAN, 122, Pixbase, Pattern, COL_DEPTH> : public DMD_RGB_BASE2<COL_DEPTH>
{
public:
  DMD_RGB(uint8_t* mux_list, byte _pin_nOE, byte _pin_SCLK, uint8_t* pinlist,
    byte panelsWide, byte panelsHigh, bool d_buf = false) :
    DMD_RGB_BASE2<COL_DEPTH>(MUX_CNT, mux_list, _pin_nOE, _pin_SCLK, pinlist,
      panelsWide, panelsHigh, d_buf, COL_DEPTH, SCAN, P_Width, P_Height)
  {
        this->fast_Hbyte = false;
        this->use_shift = false;
        }
    // Fast text shift is disabled for complex patterns, so we don't need the method
        void disableFastTextShift(bool shift) override {UNUSED1(shift);}

protected:

  uint16_t get_base_addr(int16_t& x, int16_t& y) override {
    this->transform_XY(x, y);
    uint8_t pol_y = y % this->pol_displ;
    x += (y / this->DMD_PIXELS_DOWN) * this->WIDTH;
    uint16_t base_addr = (pol_y % this->nRows) * this->x_len +
            (x / Pixbase) * this->multiplex * Pixbase + x % Pixbase;
    if ((Pattern + (x % P_Width)/Pixbase)%2)  { if (pol_y < this->nRows)  base_addr += Pixbase; }
    else { if (pol_y >=  this->nRows)  base_addr += Pixbase; }

    return base_addr;
  }

};

//--------------------------------------------------------------------------------------
template <int MUX_CNT, int P_Width, int P_Height, int SCAN, int SCAN_TYPE, int... Pars>
class [[deprecated("Use dmd.configure_multiplexer() instead")]]
DMD_RGB_SHIFTREG_ABC : public DMD_RGB<33, P_Width, P_Height, SCAN, SCAN_TYPE, Pars... >

{
public:
	DMD_RGB_SHIFTREG_ABC(uint8_t* mux_list, byte _pin_nOE, byte _pin_SCLK, uint8_t* pinlist,
		byte panelsWide, byte panelsHigh, bool d_buf = false) :
		DMD_RGB<33, P_Width, P_Height, SCAN, SCAN_TYPE, Pars... >(mux_list, _pin_nOE, _pin_SCLK, pinlist,
			panelsWide, panelsHigh, d_buf)
	{}

protected:

};
#endif
