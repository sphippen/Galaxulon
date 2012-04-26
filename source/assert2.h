// assert2.h
//
// Authors: Aric Parkinson, Spencer Phippen
// Date created: March 29, 2012
//
// Contains an assertion function for debugging purposes

#ifndef ASSERT2_H_
#define ASSERT2_H_

#undef assert2

// MACRO: assert2(bool condition, const char* message)

// If NDEBUG is defined, then the macro does nothing.
// Otherwise, if "condition" evaluates to false, then program execution is halted,
// and information about where the error occurred + your message
// is written to ASSERT_INFO_LOCATION in memory.

#define ASSERT_INFO_LOCATION 0x02020000

#ifdef NDEBUG // we can turn it off
#define assert2(__cond, __mess) ((void)0)
#else
#define assert2(__cond, __mess) ((__cond) ? (void)0 : assert2_func(__FILE__, __LINE__, __PRETTY_FUNCTION__, __mess, #__cond))
#endif

void assert2_func(const char* fileName, int lineNum, const char* funcName, const char* message, const char* condition);

#endif