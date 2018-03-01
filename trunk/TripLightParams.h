// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
        00      25dec15	initial version
 
		view parameters

*/

//			type	name				init
TLPARAMDEF(	int,	Cols,				4)	// number of columns
TLPARAMDEF(	int,	Rows,				3)	// number of rows
TLPARAMDEF(	int,	Pattern,			CTripLightView::ALT_ROWS)	// pattern bitmask; see enum
TLPARAMDEF(	int,	ColorSpeed,			90)	// color speed, in color delta per tick
TLPARAMDEF(	int,	ColorSpeedStep,		5)	// color speed step size
TLPARAMDEF(	int,	ColorSpeedVar,		6)	// color speed variation, as percentage of color speed
TLPARAMDEF(	int,	ColorSpeedVarStep,	1)	// color speed variation step size

#undef TLPARAMDEF


