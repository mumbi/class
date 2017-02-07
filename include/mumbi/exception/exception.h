#pragma once

#ifndef MUMBI__EXCEPTION__EXCEPTION__H
#define MUMBI__EXCEPTION__EXCEPTION__H

#include <stdexcept>

namespace mumbi {
namespace exception
{
	using std::logic_error;
	using std::string;

	class format_exception : public logic_error
	{
	public:
		explicit format_exception(const string& message);
		explicit format_exception(const char* message);
	};

	class network_exception : public logic_error
	{
	public:
		network_exception(int error_code, const string& message);
		network_exception(int error_code, const char* message);

		int error_code() const;

	private:
		int		_error_code;
	};

	class invalid_object_exception : public logic_error
	{
	public:
		invalid_object_exception(const string& message);
		invalid_object_exception(const char* message);
	};	
}}

#endif	// MUMBI__EXCEPTION__EXCEPTION__H
