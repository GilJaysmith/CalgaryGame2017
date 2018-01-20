#include "Pch.h"

#include "sstream"

#include <stdio.h>
#include <Windows.h>

namespace Logging
{
	void Log(const std::string& log, const std::string& message)
	{
		std::stringstream buffer;
		buffer << log;
		buffer << ": ";
		buffer << message;
		buffer << "\n";
		std::string contents(buffer.str());
		OutputDebugString(contents.c_str());
	}
}
