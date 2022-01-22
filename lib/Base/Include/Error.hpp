#pragma once 

#include "stdarg.h"

void error(const char* format);
void error(const char* format, va_list argp);