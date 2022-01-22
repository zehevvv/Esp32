#include "Error.hpp"
#include "stdio.h"
#include "Esp.h"

void error(const char* format)
{
    va_list argp;
    error(format, argp);
}

void error(const char* format, va_list argp)
{
    printf(format, argp);
    delay(1000);
    printf("\n\n\n");
    ESP.restart();
}