#pragma once

#ifndef PERFORMER__H
#define PERFORMER__H

#include <memory>

namespace mumbi {
namespace threading
{
	class performable_interface;
	using performable_interface_ptr = std::shared_ptr<performable_interface>;

	class performer
	{
	public:
		virtual ~performer() = 0 {};
	
		virtual bool post(performable_interface_ptr performable) = 0;
	};
}}

#endif	// PERFORMER__H