#pragma once

#ifndef BINARY_READER__H
#define BINARY_READER__H

#include <cstdint>
#include <cassert>
#include <string>
#include <vector>

#include "stream.h"

namespace mumbi {
namespace io
{
	using std::basic_string;

	class binary_reader
	{
		using byte_type = uint8_t;

	public:
		binary_reader(stream& stream);

		stream& get_stream();

		void close();

		size_t read(byte_type* buffer, size_t offset, size_t count);
		bool read_bool();
		int8_t read_int8();
		uint8_t read_uint8();
		int16_t read_int16();
		uint16_t read_uint16();
		int32_t read_int32();
		uint32_t read_uint32();
		int64_t read_int64();
		uint64_t read_uint64();
		float read_float();
		double read_double();	

		int read_7bit_encoded_int();		

		template<typename T, typename F>
		basic_string<T> read_basic_string(F&& encoding)
		{
			const int size = read_7bit_encoded_int();

			const int position = _stream.get_position();
			stream::iterator start = _stream.get_iterator(position);
			stream::iterator end = _stream.get_iterator(position + size);

			basic_string<T> s;
			s.reserve(size);
			encoding(s, size, start, end);
			return s;
		}

		template<typename T>
		basic_string<T> read_basic_string()
		{
			return read_basic_string<T>([](basic_string<T>& s, size_t size, stream::iterator start, stream::iterator end)
			{
				s.resize(size / sizeof(T));
				copy(start, end, reinterpret_cast<uint8_t*>(const_cast<T*>(s.data())));
			});
		}		

		template<typename T, size_t N>
		void read_array(T(&arr)[N])
		{
			_stream.read(reinterpret_cast<byte_type*>(arr), 0, sizeof(arr));
		}		

	private:
		template<typename T>
		T read_byte()
		{
			return static_cast<T>(_stream.read_byte());
		}

		template<typename T>
		T read_multibytes()
		{
			T value = 0;
			_stream.read(reinterpret_cast<byte_type*>(&value), 0, sizeof(value));			

			return value;
		}		

	private:
		stream&		_stream;
	};
}}

#endif	// BINARY_READER__H