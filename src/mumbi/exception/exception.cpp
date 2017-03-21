#include "exception.h"

namespace mumbi {
namespace exception
{
	format_exception::format_exception(const string& message)
		: format_exception(message.c_str())		
	{
	}

	format_exception::format_exception(const char* message)
		: logic_error(message)		
	{
	}

	network_exception::network_exception(int error_code, const string& message)
		: network_exception(error_code, message.c_str())
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
		: invalid_object_exception(message.c_str())		
	{
	}

	invalid_object_exception::invalid_object_exception(const char* message)
		: logic_error(message)		
	{
	}
	
	nullptr_exception::nullptr_exception(const string& message)
		: nullptr_exception(message.c_str())		
	{
	}

	nullptr_exception::nullptr_exception(const char* message)
		: logic_error(message)		
	{
	}
}}

