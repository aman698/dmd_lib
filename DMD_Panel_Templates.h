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
/*--------------------------------------------------------------------------------------*/

// P6 32x32 1/8 outdoor, BINARY mux
#define RGB32x32_S8_maxmurugan		3,32,32,8,3	// 32x32 1/8 BINARY from @maxmurugan

// 32x32 plain, single row scan (scan = height / 2)
#define RGB32x32plainS16  4,32,32,16,0		// 32x32 1/16

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
