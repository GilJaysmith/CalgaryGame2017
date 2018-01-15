#include "Pch.h"

#include <stdio.h>

namespace Logging
{
	void Log(const char* log, const char* message)
	{
		char output[16384];
		sprintf_s<sizeof(output)>(output, "%s: %s\n", log, message);
		OutputDebugString(output);
	}
}
