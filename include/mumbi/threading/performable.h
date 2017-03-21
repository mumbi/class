#pragma once

#ifndef MUMBI__THREADING__PERFORMABLE__H
#define MUMBI__THREADING__PERFORMABLE__H

#include <memory>

#include "invoker_holder.h"

namespace mumbi {
namespace threading
{
	using std::shared_ptr;

	class invoker;
	
	class performable
		: protected invoker_holder
	{
	public:
		virtual ~performable();
		performable(shared_ptr<invoker> invoker);

		void perform() const;
	};
}}

#endif	// MUMBI__THREADING__PERFORMABLE__H