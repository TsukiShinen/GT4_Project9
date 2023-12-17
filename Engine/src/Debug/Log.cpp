#include "Log.h"

#include <ctime>

#include "Platform/WindowsWindow.h"

namespace Engine
{
	void Log::Print(const LogLevel pLevel, const char* pSender, const char* pMessage, ...)
	{
		const char* levelStrings[6] = {"FATAL", "ERROR", "WARN", "INFO", "DEBUG", "TRACE"};
		const std::time_t now = std::time(nullptr);
		const std::tm* timeInfo = std::localtime(&now);
		char timestamp[9];
		std::strftime(timestamp, sizeof(timestamp), "%H:%M:%S", timeInfo);

		const bool isError = pLevel < Warn;
		
		va_list args;
		va_start(args, pMessage);
		auto message = FormatMessageV(pMessage, args);
		va_end(args);

		message = LogFormatMessage("[%s] [%s] %s: %s\n", timestamp, levelStrings[pLevel], pSender, message);

		if (isError)
			WindowsWindow::ConsoleWriteError(message, pLevel);
		else
			WindowsWindow::ConsoleWrite(message, pLevel);

		delete[] message;
	}

	char* Log::LogFormatMessage(const char* pFormat, ...)
	{
		va_list args;
		va_start(args, pFormat);
		const auto buffer = FormatMessageV(pFormat, args);
		va_end(args);

		return buffer;
	}

	char* Log::FormatMessageV(const char* pFormat, va_list pVaList)
	{
		// Calculate the size needed for the formatted string
		const int size = std::vsnprintf(nullptr, 0, pFormat, pVaList);
		if (size <= 0)
		{
			va_end(pVaList);
			return nullptr;
		}

		// Allocate a buffer and format the string
		const auto buffer = new char[size + 1];
		std::vsnprintf(buffer, size + 1, pFormat, pVaList);
		return buffer;
	}
}
