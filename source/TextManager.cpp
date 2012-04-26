// TextManager.cpp
// 
// Authors: Aric Parkinson, Spencer Phippen
// Date created: April 13, 2012
//
// -------
// Contains interface implementation for text-related functions.
//

#include "tonc_core.h"

#include "TextManager.h"
#include "globals.h"
#include "Vector.h"

void write(const char* string, Vec2 tilePos)
{
	for(int i = 0; string[i] != '\0'; ++i)
	{
		memset16(&se_mem[28][tilePos.y * 32 + tilePos.x + i], string[i] - g_galaxulonFont_Offset + g_galaxulonFont_TilesPos, 1);
	}
}

void write2(const char* string, Vec2 tilePos)
{
	for(int i = 0; string[i] != '\0'; ++i)
	{
		memset16(&se_mem[29][tilePos.y * 32 + tilePos.x + i], string[i] - g_galaxulonFont_Offset + g_galaxulonFont_TilesPos, 1);
	}
}

void clearText()
{
	memset16(&se_mem[28], 0x0000, 32 * 32);
}

void clearText2()
{
	memset16(&se_mem[29], 0x0000, 32 * 32);
}

void gba_itoa(int value, char* str)
{
	// write a negative sign as appropriate
	if (value < 0)
		*str++ = '-';

	// this is the address of the first digit written
	char* begDig = str;

	int numDigs = 0;
	if (value == 0)
	{
		// write a '0'
		*str++ = value + 0x30;
		++numDigs;
	}
	else
	{
		// "number" % 10 gives the last digit
		// after writing that, we can divide "number"
		// by 10 to "shift right" by one decimal digit, and
		// continue the process until all digits are written
		while (value > 0)
		{
			*str++ = (value % 10) + 0x30;
			++numDigs;
			value /= 10;
		}
	}

	// this is the address of the last digit written
	char* endDig = str-1;

	// the above code outputs the digits in "number" starting with the least significant
	// digit, so now we swap digits from the front and end until they
	// are in the correct order
	for (int i = 0; i < numDigs/2; ++i, ++begDig, --endDig)
	{
		char tmp = *begDig;
		*begDig = *endDig;
		*endDig = tmp;
	}

	// null terminate string
	*str++ = '\0';
}
