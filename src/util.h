#ifndef _h_private_util
#define _h_private_util

#include <string>
#include <vector>

#ifdef DEBUG
struct Logger
{
	std::vector<std::pair<size_t, std::string>> messages;
	size_t level = 0;

	void log(const std::string& message)
	{
		messages.push_back({ level, message });
	}

	std::string str() const
	{
		std::string result;
		for (auto& entry : messages)
		{
			std::string fill((entry.first - 1) * 4, ' ');
			result += fill + entry.second + "\n";
		}
		return result;
	}
};

inline Logger _debug_logger;

struct LoggerContext
{
	LoggerContext() { _debug_logger.level++; }
	~LoggerContext() { _debug_logger.level--; }
};

#	define DEBUG_LOG(what) _debug_logger.log(what);
#	define DEBUG_CONTEXT LoggerContext _debug_logger_context;
#	define DEBUG_LOG_GETSTR() _debug_logger.str()
#else
#	define DEBUG_LOG(what)
#	define DEBUG_CONTEXT
#	define DEBUG_PRINT_LOG()
#endif

#endif