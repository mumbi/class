#include "buffer.h"

#include "../exception/exception.h"

namespace mumbi {
namespace io
{
	using std::make_shared;
	using mumbi::exception::nullptr_exception;

	buffer::buffer()		
	{
	}

	buffer::buffer(size_t size)
		: _data(make_shared<data_type>(size))
	{
	}
	
	buffer::data_type& buffer::data()
	{
		if (!_data)
			throw nullptr_exception("_data is null.");		

		return *_data;
	}

	const buffer::data_type& buffer::data() const
	{
		return const_cast<buffer*>(this)->data();
	}	
}}
