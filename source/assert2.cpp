// assert2.cpp
//
// Authors: Spencer Phippen
// Date created: March 29, 2012
//
// Contains an assertion function for debugging purposes

#include "tonc_core.h"
#include "assert2.h"

// Copys characters from "src" into "dest" until a null
// character is reached, and returns a pointer to the character
// just past the last one written
char* cpyNultermString(char* dest, const char* src);

// Writes "number" to "dest" in decimal form, and returns a pointer
// to the character just past the last one written
// DOES NOT WORK FOR NEGATIVE NUMBERS
char* outIntAsDecimal(char* dest, int number);

// Prints information about assertion failure to ASSERT_INFO_LOCATION
// See assert2.h
void assert2_func(const char* fileName, int lineNum, const char* funcName, const char* message, const char* condition)
{
	char* dest = reinterpret_cast<char*>(ASSERT_INFO_LOCATION);

	// set background to the assert-failure tile
	memset32(&se_mem[24], 0x00010001, SBB_SIZE);

	dest = cpyNultermString(dest, fileName);
	dest = cpyNultermString(dest, ":");
	dest = outIntAsDecimal(dest, lineNum);
	dest = cpyNultermString(dest, " - ");
	dest = cpyNultermString(dest, funcName);
	dest = cpyNultermString(dest, " failed assertion: ");
	dest = cpyNultermString(dest, condition);
	dest = cpyNultermString(dest, " message: ");
	dest = cpyNultermString(dest, message);

	// this function never returns
	while(1);
}

char* cpyNultermString(char* dest, const char* src)
{
	while (*src != '\0')
	{
		*dest++ = *src++;
	}
	return dest;
}

char* outIntAsDecimal(char* dest, int number)
{
	// write a negative sign as appropriate
	if (number < 0)
		*dest++ = '-';

	// this is the address of the first digit written
	char* begDig = dest;

	int numDigs = 0;
	if (number == 0)
	{
		// write a '0'
		*dest++ = number + 0x30;
		++numDigs;
	}
	else
	{
		// "number" % 10 gives the last digit
		// after writing that, we can divide "number"
		// by 10 to "shift right" by one decimal digit, and
		// continue the process until all digits are written
		while (number > 0)
		{
			*dest++ = (number % 10) + 0x30;
			++numDigs;
			number /= 10;
		}
	}

	// this is the address of the last digit written
	char* endDig = dest-1;

	// the above code outputs the digits in "number" starting with the least significant
	// digit, so now we swap digits from the front and end until they
	// are in the correct order
	for (int i = 0; i < numDigs/2; ++i, ++begDig, --endDig)
	{
		char tmp = *begDig;
		*begDig = *endDig;
		*endDig = tmp;
	}

	return dest;
}