#pragma once

#ifndef MUMBI__THREADING__INVOKER__H
#define MUMBI__THREADING__INVOKER__H

namespace mumbi {
namespace threading
{
	class invoker
	{
	public:		
		virtual ~invoker() = 0 {}
		virtual bool invoke() = 0;
	};
}}

#endif	// MUMBI__THREADING__INVOKER__H