#pragma once

#ifndef MUMBI__IO__BUFFER__H
#define MUMBI__IO__BUFFER__H

#include <vector>
#include <memory>

#include "../threading/spin_lock.h"

namespace mumbi {
namespace io
{
	using std::vector;
	using std::shared_ptr;

	class buffer
	{
	public:
		using data_type = vector<uint8_t>;
		using iterator = data_type::iterator;
		using const_iterator = data_type::const_iterator;

	public:
		explicit buffer();
		explicit buffer(size_t size);

		data_type& data();
		const data_type& data() const;

	private:
		shared_ptr<data_type>	_data;
	};
}}

#endif	// MUMBI__IO__BUFFER__H
