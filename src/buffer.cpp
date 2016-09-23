#include "buffer.h"

namespace mumbi {
namespace io
{
	buffer_ptr make_buffer(size_t size /*= 0*/)
	{
		return buffer_ptr(size > 0 ? new buffer_type(size) : new buffer_type);
	}
}}
