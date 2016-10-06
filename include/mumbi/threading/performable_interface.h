#pragma once

#ifndef MUMBI__THREADING__PERFORMABLE_INTERFACE__H
#define MUMBI__THREADING__PERFORMABLE_INTERFACE__H

namespace mumbi {
namespace threading
{
	class performable_interface
	{
	public:
		virtual void perform() = 0;		
	};
}}

#endif	// MUMBI__THREADING__PERFORMABLE_INTERFACE__H