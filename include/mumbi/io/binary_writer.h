#pragma once

#ifndef MUMBI__IO__BINARY_WRITER__H
#define MUMBI__IO__BINARY_WRITER__H

#include <cstdint>
#include <string>
#include <vector>

namespace mumbi {
namespace io
{
	using std::basic_string;

	class stream;
	enum class seek_origin;

	class binary_writer
	{
		using byte_type = uint8_t;

	public:
		binary_writer(stream& stream);

		void close();
		void flush();
		void seek(int position, seek_origin origin);

		size_t write(byte_type* buffer, size_t offset, size_t count);
		void write(bool value);
		void write(int8_t value);
		void write(uint8_t value);
		void write(int16_t value);
		void write(uint16_t value);
		void write(int32_t value);
		void write(uint32_t value);
		void write(int64_t value);
		void write(uint64_t value);
		void write(float value);
		void write(double value);

		void write_7bit_encoded_int(int value);
		static size_t size_of_7bit_encoded_int(int value);

		template<typename T, typename F>
		void write(const basic_string<T>& value, F&& encoding)
		{
			vector<uint8_t> encoded;
			encoding(value, encoded);

			const size_t size = encoded.size();
			write_7bit_encoded_int(size);
			write(encoded.data(), 0, size);
		}
		
		template<typename T>
		void write(const basic_string<T>& value)
		{
			write(value, [this](const basic_string<T>& src, vector<uint8_t>& dest)
			{
				const size_t size = sizeof(T) * src.size();
				dest.resize(size);
				uint8_t* start = reinterpret_cast<uint8_t*>(const_cast<T*>(src.data()));
				copy(start, start + size, dest.begin());				
			});
		}	

		template<typename T, size_t N>
		void write(const T(&arr)[N])
		{
			_stream.write(reinterpret_cast<const byte_type*>(arr), 0, sizeof(arr));
		}

	private:
		template<typename T>
		void write_bytes(T value)
		{
			_stream.write(reinterpret_cast<const byte_type*>(&value), 0, sizeof(value));
		}	

	private:
		stream&		_stream;
	};
}}

#endif	// MUMBI__IO__BINARY_WRITER__H