#pragma once

#ifndef MUMBI__THREADING__PERFORMER__H
#define MUMBI__THREADING__PERFORMER__H

#include <memory>

namespace mumbi {
namespace threading
{
	class performable;	

	class performer
	{
	public:
		virtual ~performer() = 0 {};	
		
		virtual void post(const performable& performable) = 0;
	};
}}

#endif	// MUMBI__THREADING__PERFORMER__H