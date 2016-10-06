#pragma once

#ifndef MUMBI__IO__BUFFER__H
#define MUMBI__IO__BUFFER__H

#include <vector>
#include <memory>

namespace mumbi {
namespace io
{
	using buffer_type = std::vector<uint8_t>;
	using buffer_ptr = std::shared_ptr<buffer_type>;

	buffer_ptr make_buffer(size_t size = 0);
}}

#endif	// MUMBI__IO__BUFFER__H
