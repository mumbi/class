#pragma once
#pragma warning(disable:4996)

#ifndef MUMBI__IO__MEMORY_STREAM__H
#define MUMBI__IO__MEMORY_STREAM__H

#include "stream.h"
#include "buffer.h"

#include <vector>
#include <memory>

namespace mumbi {
namespace io
{
	using std::vector;
	using std::out_of_range;
	using std::invalid_argument;

	class memory_stream
		: public stream
	{
	private:
		static constexpr size_t	default_capacity = 256;		

		using data_type = buffer::data_type;

	public:
		memory_stream();
		memory_stream(size_t initial_capacity);
		memory_stream(const buffer& buffer);
		memory_stream(const vector<buffer>& buffers);

		void close() override;
		size_t get_length() const override;
		void set_length(size_t length) override;
		size_t get_position() const override;
		void set_position(size_t value) override;
		void flush() override;
		size_t peek(uint8_t* buffer, size_t offset, size_t count) const override;
		uint8_t peek_byte() const override;
		uint8_t& peek_byte() override;
		size_t read(uint8_t* buffer, size_t offset, size_t count) override;
		uint8_t read_byte() override;
		size_t write(const uint8_t* buffer, size_t offset, size_t count) override;
		void write_byte(uint8_t byte) override;
		size_t seek(int position, seek_origin origin) override;

		size_t available_size() const;		
		const vector<buffer>& buffers() const;

	private:		
		void increase_buffer(size_t capacity);		
		
		size_t available_size_in_buffer(size_t index, size_t position) const;		
		
		void advance_position(size_t& index, size_t& position, int step) const;
		//void advance_position(size_t& index, size_t& position, int step);
		
		size_t copy_to(size_t& index, size_t& position, uint8_t* buffer, size_t offset, size_t count) const;
		size_t copy_to(size_t& index, size_t& position, uint8_t* buffer, size_t offset, size_t count);

		buffer::iterator position_to_iterator(size_t index, size_t position);

	private:
		vector<buffer>	_buffers;
		
		size_t			_index;
		size_t			_position;
	};
}}

#endif	// MUMBI__IO__MEMORY_STREAM__H