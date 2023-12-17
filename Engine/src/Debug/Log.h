#pragma once
#include <fstream>
#include <cstdarg>

namespace Engine
{
	/**
	 * \brief Represents levels of logging
	 */
	enum LogLevel
	{
		Critical,
		Error,
		Warn,
		Info,
		Debug,
		Trace
	};

	class Log
	{
	public:
		/**
		 * \brief Outputs logging at the given level.
		 * \param pLevel The log level to use.
		 * \param pSender Name of the sender.
		 * \param pMessage The message to be logged.
		 * \param ... Any formatted data that should be included in the log entry.
		 */
		static void Print(LogLevel pLevel, const char* pSender, const char* pMessage, ...);

	private:
		static char* LogFormatMessage(const char* pFormat, ...);
		static char* FormatMessageV(const char* pFormat, va_list pVaList);
	};
}

// Core log macros

/**
 * \brief Logs a trace-level message. Should be used for verbose debugging purposes.
 * \param pMessage The message to be logged.
 * \param ... Any formatted data that should be included in the log entry.
 */
#define CORE_TRACE(pMessage, ...)    ::Engine::Log::Print(::Engine::LogLevel::Trace, "Engine", pMessage, ##__VA_ARGS__)
/**
 * \brief Logs a debug-level message. Should be used for debugging purposes.
 * \param pMessage The message to be logged.
 * \param ... Any formatted data that should be included in the log entry.
 */
#define CORE_DEBUG(pMessage, ...)     ::Engine::Log::Print(::Engine::LogLevel::Debug, "Engine", pMessage, ##__VA_ARGS__)
/**
 * \brief Logs an info-level message. Should be used for non-erroneous informational purposes.
 * \param pMessage The message to be logged.
 * \param ... Any formatted data that should be included in the log entry.
 */
#define CORE_INFO(pMessage, ...)     ::Engine::Log::Print(::Engine::LogLevel::Info, "Engine", pMessage, ##__VA_ARGS__)
/**
* \brief Logs a warning-level message. Should be used to indicate non-critical problems with
 * the application that cause it to run suboptimally.
 * \param pMessage The message to be logged.
 * \param ... Any formatted data that should be included in the log entry.
 */
#define CORE_WARN(pMessage, ...)     ::Engine::Log::Print(::Engine::LogLevel::Warn, "Engine", pMessage, ##__VA_ARGS__)
/**
* \brief Logs an error-level message. Should be used to indicate critical runtime problems
 * that cause the application to run improperly or not at all.
 * \param pMessage The message to be logged.
 * \param ... Any formatted data that should be included in the log entry.
 */
#define CORE_ERROR(pMessage, ...)    ::Engine::Log::Print(::Engine::LogLevel::Error, "Engine", pMessage, ##__VA_ARGS__)
/**
 * \brief Logs a fatal-level message. Should be used to stop the application when hit.
 * \param pMessage The message to be logged.
 * \param ... Any formatted data that should be included in the log entry.
 */
#define CORE_CRITICAL(pMessage, ...) ::Engine::Log::Print(::Engine::LogLevel::Critical, "Engine", pMessage, ##__VA_ARGS__)

// Client log macros
/**
 * \brief Logs a debug-level message. Should be used for debugging purposes.
 * \param pMessage The message to be logged.
 * \param ... Any formatted data that should be included in the log entry.
 */
#define TRACE(pMessage, ...)    ::Engine::Log::Print(::Engine::LogLevel::Trace, "Game", pMessage, ##__VA_ARGS__)
/**
 * \brief Logs an info-level message. Should be used for non-erroneous informational purposes.
 * \param pMessage The message to be logged.
 * \param ... Any formatted data that should be included in the log entry.
 */
#define DEBUG_LOG(pMessage, ...)     ::Engine::Log::Print(::Engine::LogLevel::Debug, "Game", pMessage, ##__VA_ARGS__)
/**
 * \brief Logs an info-level message. Should be used for non-erroneous informational purposes.
 * \param pMessage The message to be logged.
 * \param ... Any formatted data that should be included in the log entry.
 */
#define INFO(pMessage, ...)     ::Engine::Log::Print(::Engine::LogLevel::Info, "Game", pMessage, ##__VA_ARGS__)
/**
* \brief Logs a warning-level message. Should be used to indicate non-critical problems with
 * the application that cause it to run suboptimally.
 * \param pMessage The message to be logged.
 * \param ... Any formatted data that should be included in the log entry.
 */
#define WARN(pMessage, ...)     ::Engine::Log::Print(::Engine::LogLevel::Warn, "Game", pMessage, ##__VA_ARGS__)
/**
* \brief Logs an error-level message. Should be used to indicate critical runtime problems
 * that cause the application to run improperly or not at all.
 * \param pMessage The message to be logged.
 * \param ... Any formatted data that should be included in the log entry.
 */
#define ERROR(pMessage, ...)    ::Engine::Log::Print(::Engine::LogLevel::Error, "Game", pMessage, ##__VA_ARGS__)
/**
 * \brief Logs a fatal-level message. Should be used to stop the application when hit.
 * \param pMessage The message to be logged.
 * \param ... Any formatted data that should be included in the log entry.
 */
#define CRITICAL(pMessage, ...) ::Engine::Log::Print(::Engine::LogLevel::Critical, "Game", pMessage, ##__VA_ARGS__)
