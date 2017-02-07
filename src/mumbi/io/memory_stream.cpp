#include "memory_stream.h"

#include <algorithm>

#include <stdexcept>

namespace mumbi {
namespace io
{
	using std::make_shared;
	using std::exception;
	using std::min;

	memory_stream::memory_stream()
		: _index(0)
		, _position(0)
	{	
		_buffers.push_back(make_shared<buffer_type>(default_capacity));
	}

	memory_stream::memory_stream(size_t initial_capacity)
		: _index(0)
		, _position(0)
	{	
		_buffers.push_back(make_shared<buffer_type>(initial_capacity));		
	}

	memory_stream::memory_stream(buffer_ptr buffer)		
		: _index(0)
		, _position(0)
	{		
		_buffers.push_back(buffer);		
	}	

	memory_stream::memory_stream(vector<buffer_ptr> buffers)
		: _buffers(buffers)
		, _index(0)		
		, _position(0)
	{
	}

	void memory_stream::close()
	{
	}	

	size_t memory_stream::get_length() const
	{
		size_t length = 0;
		for (const buffer_ptr& buffer : _buffers)
		{
			length += buffer->size();
		}

		return length;
	}

	void memory_stream::set_length(size_t length)
	{
		int distance = get_length() - length;

		if (0 == distance)
			return;

		if (distance > 0)
		{
			while (distance > 0)
			{
				buffer_ptr last_buffer = _buffers.back();
				size_t last_buffer_size = last_buffer->size();

				size_t remove_size = 0;
				
				const int distance_from_last_buffer = last_buffer_size - distance;
				if (distance_from_last_buffer > 0)
				{
					last_buffer->resize(distance_from_last_buffer);				

					if (static_cast<size_t>(_index) >= _buffers.size() - 1 && static_cast<size_t>(_position) > last_buffer->size())
						_position = last_buffer->size();

					remove_size = distance;
				}
				else
				{
					_buffers.pop_back();

					--_index;					
					_position = _index >= 0 ? _buffers[_index]->size() : 0;					

					remove_size = last_buffer_size;
				}

				distance -= remove_size;
			}
		}
		else
		{
			int capacity = -distance;
			increase_buffer(capacity);			
		}
	}

	int memory_stream::get_position() const
	{
		int position = 0;
		for (int i = 0; i < _index; ++i)
		{
			position += _buffers[i]->size();
		}
		
		return position + _position;
	}

	void memory_stream::set_position(size_t value)
	{
		//size_t position = value;

		//_index = 0;
		//while (position >= _buffers[_index]->size())
		//{
		//	position -= _buffers[_index]->size();
		//	++_index;
		//}

		//_position = position;
		seek(value, mumbi::io::seek_origin::begin);
	}

	void memory_stream::flush()
	{
	}

	size_t memory_stream::peek(uint8_t* buffer, size_t offset, size_t count) const
	{
		int index = _index;		
		int position = _position;
		
		return copy_to(index, position, buffer, offset, count);
	}

	uint8_t memory_stream::peek_byte() const
	{
		return const_cast<memory_stream*>(this)->peek_byte();		
	}

	uint8_t& memory_stream::peek_byte()
	{	
		buffer_type& buffer = *_buffers[_index];
		if (buffer.size() <= static_cast<size_t>(_position))
			throw out_of_range("end stream.");
		
		return buffer[_position];
	}

	size_t memory_stream::read(uint8_t* buffer, size_t offset, size_t count)
	{		
		return copy_to(_index, _position, buffer, offset, count);
	}

	uint8_t memory_stream::read_byte()
	{		
		const buffer_type& buffer = *_buffers[_index];
		if (buffer.size() <= static_cast<size_t>(_position))
			throw out_of_range("end stream.");
	
		uint8_t byte = buffer[_position];	
		advance_position(_index, _position, 1);

		return byte;
	}

	size_t memory_stream::write(const uint8_t* buffer, size_t offset, size_t count)
	{
		size_t write_count = count;		

		while (write_count > 0)
		{	
			const size_t available = available_size_in_buffer(_index, _position);
			if (0 >= available)
				increase_buffer(default_capacity);

			size_t writing = write_count < available ? write_count : available;
			
			copy(buffer + offset, buffer + offset + writing, position_to_iterator(_index, _position));
			
			advance_position(_index, _position, writing);

			write_count -= writing;
			offset += writing;
		}	

		return count;
	}

	void memory_stream::write_byte(uint8_t byte)
	{
		write(&byte, 0, sizeof(byte));		
	}

	int memory_stream::seek(int position, seek_origin origin)
	{
		switch (origin)
		{
		case seek_origin::begin:
			{
				if (position < 0)
					throw invalid_argument("position is negative.");
				
				_index = 0;				
				_position = 0;
				
				advance_position(_index, _position, position);
			}
			
			break;
		case seek_origin::current:
			{				
				advance_position(_index, _position, position);
			}
			
			break;
		case seek_origin::end:
			{
				if (position > 0)
					throw invalid_argument("position is positive.");

				_index = _buffers.size() - 1;				
				_position = _buffers[_index]->size();
				
				advance_position(_index, _position, position);
			}
			
			break;
		}

		return this->get_position();
	}	

	size_t memory_stream::available_size() const
	{		
		size_t size = available_size_in_buffer(_index, _position);

		for (size_t i = _index + 1; i < _buffers.size(); ++i)
		{
			size += _buffers[i]->size();
		}

		return size;
	}	

	const vector<memory_stream::buffer_ptr>& memory_stream::buffers() const
	{
		return _buffers;
	}

	void memory_stream::increase_buffer(size_t capacity)
	{
		_buffers.push_back(make_shared<buffer_type>(capacity));

		if (_index < 0)
		{
			_index = 0;			
			_position = 0;
		}
		else
		{			
			advance_position(_index, _position, -1);			
			advance_position(_index, _position, 1);
		}
	}	
	
	size_t memory_stream::available_size_in_buffer(int index, int position) const
	{
		return _buffers[index]->size() - position;		
	}	
	
	void memory_stream::advance_position(int& index, int& position, int step) const
	{
		return const_cast<memory_stream*>(this)->advance_position(index, position, step);
	}
	
	void memory_stream::advance_position(int& index, int& position, int step)
	{
		if (0 == step)
			return;

		if (step > 0)
		{			
			size_t available = available_size_in_buffer(index, position);
			while (step > 0 && static_cast<size_t>(step) >= available)
			{
				if (static_cast<size_t>(index) >= _buffers.size() - 1)
				{
					if (0 == available)
						throw out_of_range("out of range.");
					
					position = _buffers[index]->size();
				}
				else
				{
					++index;
					position = 0;
				}
				
				step -= available;
				available = available_size_in_buffer(index, position);
			}			
			
			position += step;
		}
		else
		{
			step = -step;

			int p = position;
			while (step > 0 && step > p)
			{
				if (index <= 0)
					throw out_of_range("out of range.");
				
				position = _buffers[--index]->size();
				step -= p;
				p = position;
			}			
			
			position -= step;
		}		
	}
	
	size_t memory_stream::copy_to(int& index, int& position, uint8_t* buffer, size_t offset, size_t count) const
	{
		return const_cast<memory_stream*>(this)->copy_to(index, position, buffer, offset, count);
	}
	
	size_t memory_stream::copy_to(int& index, int& position, uint8_t* buffer, size_t offset, size_t count)
	{
		const size_t readable_count = min(available_size(), count);
		size_t read_count = readable_count;

		while (read_count > 0)
		{			
			const size_t available = available_size_in_buffer(index, position);
			size_t reading = read_count < available ? read_count : available;
			
			copy(position_to_iterator(index, position), position_to_iterator(index, position + reading), buffer + offset);
			
			advance_position(index, position, reading);

			read_count -= reading;
			offset += reading;
		}

		return readable_count;
	}

	memory_stream::buffer_type::iterator memory_stream::position_to_iterator(int index, int position) const
	{
		return begin(*_buffers[index]) + position;
	}
}}
