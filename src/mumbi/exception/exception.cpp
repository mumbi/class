#include "exception.h"

namespace mumbi {
namespace exception
{
	format_exception::format_exception(const string& message)
		: logic_error(message)
	{
	}

	format_exception::format_exception(const char* message)
		: logic_error(message)
	{
	}

	network_exception::network_exception(int error_code, const string& message)
		: logic_error(message)
		, _error_code(error_code)
	{
	}

	network_exception::network_exception(int error_code, const char* message)
		: logic_error(message)
		, _error_code(error_code)
	{
	}

	int network_exception::error_code() const
	{
		return _error_code;
	}

	invalid_object_exception::invalid_object_exception(const string& message)
		: logic_error(message)
	{
	}

	invalid_object_exception::invalid_object_exception(const char* message)
		: logic_error(message)
	{
	}
}}

