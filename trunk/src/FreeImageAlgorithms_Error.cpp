#include "FreeImageAlgorithms.h"
#include "FreeImageAlgorithms_Error.h"

#include <stdio.h>

static FreeImageAlgorithms_OutputMessageFunction errorFunction;

void DLL_CALLCONV
FreeImageAlgorithms_SetOutputMessage(FreeImageAlgorithms_OutputMessageFunction omf)
{
	errorFunction = omf;
}

void DLL_CALLCONV
FreeImageAlgorithms_SendOutputMessage(const char *fmt, ...)
{
	va_list ap;
	char message[500];
	
    va_start(ap, fmt);

	vsprintf(message, fmt, ap);

	va_end(ap);

	errorFunction(message);
}