#include "string_helper.h"

namespace mumbi {
namespace string
{
	using std::wstring_convert;
	using std::codecvt;
	
	wstring_convert<codecvt<wchar_t, char, mbstate_t>, wchar_t> converter;

	wstring string_to_wstring(const string& str)
	{
		return converter.from_bytes(str);
	}

	string wstring_to_string(const wstring& str)
	{
		return converter.to_bytes(str);
	}	
}}