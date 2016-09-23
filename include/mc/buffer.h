#pragma once

#ifndef BUFFER__H
#define BUFFER__H

#include <vector>
#include <memory>

namespace mumbi {
namespace io
{
	using buffer_type = std::vector<uint8_t>;
	using buffer_ptr = std::shared_ptr<buffer_type>;

	buffer_ptr make_buffer(size_t size = 0);
}}

#endif	// BUFFER__H
