#pragma once

#ifndef MUMBI__UTIL__STRING_HELPER__H
#define MUMBI__UTIL__STRING_HELPER__H

#include <string>
#include <codecvt>
#include <vector>

namespace mumbi {
namespace util
{
	using std::wstring;
	using std::string;		
	using std::vector;
	using std::basic_string;

	wstring string_to_wstring(const string& str);
	string wstring_to_string(const wstring& str);

	template<typename T>
	static vector<basic_string<T>> tokenize(const basic_string<T>& text, const T* delimiters)
	{
		vector<basic_string<T>> tokens;

		const size_t text_length = text.length();

		size_t start = text.find_first_not_of(delimiters);
		while ((start >= 0) && (start < text_length))
		{
			size_t end = text.find_first_of(delimiters, start);
			if ((end < 0) || end > text_length)
				end = text_length;

			basic_string<T> token = text.substr(start, end - start);
			if (false == token.empty())
				tokens.push_back(token);

			start = text.find_first_not_of(delimiters, end + 1);
		}

		return tokens;
	}
}}

#endif	// MUMBI__UTIL__STRING_HELPER__H

