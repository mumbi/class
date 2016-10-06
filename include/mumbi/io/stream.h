#pragma once

#ifndef MUMBI__IO__STREAM__H
#define MUMBI__IO__STREAM__H

#include <cstdint>
#include <iterator>

namespace mumbi {
namespace io
{
	using std::forward_iterator_tag;

	enum class seek_origin
	{
		begin,
		current,
		end,
	};	

	class stream
	{
	public:
		class iterator
		{
		public:
			using value_type		= uint8_t;
			using difference_type	= int;
			using iterator_category = forward_iterator_tag;
			using pointer			= value_type*;
			using reference			= value_type&;

		public:
			iterator(stream& stream, int position);

			iterator& operator=(const iterator& other);
			bool operator==(const iterator& other);
			bool operator!=(const iterator& other);

			value_type& operator*();

			iterator& operator++();
			iterator operator++(int);

		private:
			stream&	_stream;
			int		_position;
		};

	public:
		virtual ~stream() = 0 {};
		virtual void close() = 0;
		virtual size_t get_length() const = 0;
		virtual void set_length(size_t length) = 0;
		virtual int get_position() const = 0;
		virtual void set_position(size_t value) = 0;
		virtual void flush() = 0;
		virtual size_t peek(uint8_t* buffer, size_t offset, size_t count) const = 0;
		virtual uint8_t peek_byte() const = 0;
		virtual uint8_t& peek_byte() = 0;
		virtual size_t read(uint8_t* buffer, size_t offset, size_t count) = 0;
		virtual uint8_t read_byte() = 0;
		virtual size_t write(const uint8_t* buffer, size_t offset, size_t count) = 0;
		virtual void write_byte(uint8_t byte) = 0;
		virtual int seek(int position, seek_origin origin) = 0;

		iterator get_iterator(int position);
	};
}}

#endif	// MUMBI__IO__STREAM__H