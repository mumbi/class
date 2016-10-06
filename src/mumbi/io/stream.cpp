#include "stream.h"

namespace mumbi {
namespace io
{
	// iterator
	stream::iterator::iterator(stream& stream, int position)
		: _stream(stream)
		, _position(position)
	{
	}

	stream::iterator& stream::iterator::operator=(const iterator& other)
	{
		_stream = other._stream;
		_position = other._position;

		return *this;
	}

	bool stream::iterator::operator==(const iterator& other)
	{
		return &_stream == &other._stream && _position == other._position;
	}

	bool stream::iterator::operator!=(const iterator& other)
	{
		return !operator==(other);
	}

	stream::iterator::value_type& stream::iterator::operator*()
	{
		return _stream.peek_byte();
	}

	stream::iterator& stream::iterator::operator++()
	{
		_position = _stream.seek(1, seek_origin::current);
		return *this;
	}

	stream::iterator stream::iterator::operator++(int)
	{
		iterator clone(_stream, _position);
		operator++();
		return clone;
	}

	// stream
	stream::iterator stream::get_iterator(int position)
	{
		return iterator(*this, position);
	}
}}