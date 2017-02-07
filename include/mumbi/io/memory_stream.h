#pragma once

#ifndef MUMBI__IO__MEMORY_STREAM__H
#define MUMBI__IO__MEMORY_STREAM__H

#include "stream.h"

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

		using buffer_type	= vector<uint8_t>;
		using buffer_ptr	= std::shared_ptr<buffer_type>;

	public:
		memory_stream();
		memory_stream(size_t initial_capacity);
		memory_stream(buffer_ptr buffer);
		memory_stream(vector<buffer_ptr> buffers);

		void close() override;
		size_t get_length() const override;
		void set_length(size_t length) override;
		int get_position() const override;
		void set_position(size_t value) override;
		void flush() override;
		size_t peek(uint8_t* buffer, size_t offset, size_t count) const override;
		uint8_t peek_byte() const override;
		uint8_t& peek_byte() override;
		size_t read(uint8_t* buffer, size_t offset, size_t count) override;
		uint8_t read_byte() override;
		size_t write(const uint8_t* buffer, size_t offset, size_t count) override;
		void write_byte(uint8_t byte) override;
		int seek(int position, seek_origin origin) override;

		size_t available_size() const;		
		const vector<buffer_ptr>& buffers() const;

	private:		
		void increase_buffer(size_t capacity);		
		
		size_t available_size_in_buffer(int index, int position) const;		
		
		void advance_position(int& index, int& position, int step) const;
		void advance_position(int& index, int& position, int step);
		
		size_t copy_to(int& index, int& position, uint8_t* buffer, size_t offset, size_t count) const;
		size_t copy_to(int& index, int& position, uint8_t* buffer, size_t offset, size_t count);

		buffer_type::iterator position_to_iterator(int index, int position) const;

	private:
		vector<buffer_ptr>	_buffers;
		
		int					_index;
		int					_position;
	};
}}

#endif	// MUMBI__IO__MEMORY_STREAM__H