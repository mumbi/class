#include "binary_writer.h"

#include "stream.h"

namespace mumbi {
namespace io
{
	binary_writer::binary_writer(stream& stream)
		: _stream(stream)
	{
	}

	void binary_writer::close()
	{
		_stream.close();
	}

	void binary_writer::flush()
	{
		_stream.flush();
	}

	void binary_writer::seek(int position, seek_origin origin)
	{
		_stream.seek(position, origin);
	}

	size_t binary_writer::write(byte_type* buffer, size_t offset, size_t count)
	{
		return _stream.write(buffer, offset, count);
	}

	void binary_writer::write(bool value)
	{
		write_bytes(value);
	}

	void binary_writer::write(int8_t value)
	{
		write_bytes(value);
	}

	void binary_writer::write(uint8_t value)
	{
		write_bytes(value);
	}

	void binary_writer::write(int16_t value)
	{
		write_bytes(value);
	}

	void binary_writer::write(uint16_t value)
	{
		write_bytes(value);
	}

	void binary_writer::write(int32_t value)
	{
		write_bytes(value);
	}

	void binary_writer::write(uint32_t value)
	{
		write_bytes(value);
	}

	void binary_writer::write(int64_t value)
	{
		write_bytes(value);
	}

	void binary_writer::write(uint64_t value)
	{
		write_bytes(value);
	}

	void binary_writer::write(float value)
	{
		write_bytes(value);
	}

	void binary_writer::write(double value)
	{
		write_bytes(value);
	}	

	void binary_writer::write_7bit_encoded_int(int value)
	{
		uint32_t i = (uint32_t)value;

		while (i >= 128)
		{
			write((uint8_t)(i | 128));
			i >>= 7;
		}

		write((uint8_t)i);
	}

	size_t binary_writer::size_of_7bit_encoded_int(int value)
	{
		size_t size = 0;
		uint32_t i = (uint32_t)value;

		while (i >= 128)
		{
			++size;
			i >>= 7;
		}

		return ++size;
	}
}}