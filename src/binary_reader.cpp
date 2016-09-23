#include "binary_reader.h"

#include <limits>
#include <exception>

#include "stream.h"

namespace mumbi {
namespace io
{
	using std::numeric_limits;
	using std::exception;

	binary_reader::binary_reader(stream& stream)
		: _stream(stream)
	{
	}

	stream& binary_reader::get_stream()
	{
		return _stream;
	}

	void binary_reader::close()
	{
		_stream.close();
	}

	size_t binary_reader::read(byte_type* buffer, size_t offset, size_t count)
	{
		return _stream.read(buffer, offset, count);
	}

	bool binary_reader::read_bool()
	{
		return 0 != _stream.read_byte();
	}

	int8_t binary_reader::read_int8()
	{
		return read_byte<int8_t>();
	}

	uint8_t binary_reader::read_uint8()
	{
		return read_byte<uint8_t>();
	}

	int16_t binary_reader::read_int16()
	{
		return read_multibytes<int16_t>();
	}

	uint16_t binary_reader::read_uint16()
	{
		return read_multibytes<uint16_t>();
	}

	int32_t binary_reader::read_int32()
	{
		return read_multibytes<int32_t>();
	}

	uint32_t binary_reader::read_uint32()
	{
		return read_multibytes<uint32_t>();
	}

	int64_t binary_reader::read_int64()
	{
		return read_multibytes<int64_t>();
	}

	uint64_t binary_reader::read_uint64()
	{
		return read_multibytes<uint64_t>();
	}

	float binary_reader::read_float()
	{
		return read_multibytes<float>();
	}

	double binary_reader::read_double()
	{
		return read_multibytes<double>();
	}

	int binary_reader::read_7bit_encoded_int()
	{
		int return_value = 0;
		int bit_index = 0;

		while (35 != bit_index)
		{
			uint8_t current_byte = read_uint8();
			return_value |= ((int)current_byte & (int)numeric_limits<int8_t>::max()) << bit_index;
			bit_index += 7;

			if (((int)current_byte & 128) == 0)
				return return_value;			
		}

		throw exception("format_bad7bitint32");
	}	
}}