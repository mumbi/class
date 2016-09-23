#pragma once

#ifndef PERFORMABLE_INTERFACE__H
#define PERFORMABLE_INTERFACE__H

namespace mumbi {
namespace threading
{
	class performable_interface
	{
	public:
		virtual void perform() = 0;		
	};
}}

#endif	// PERFORMABLE_INTERFACE__H