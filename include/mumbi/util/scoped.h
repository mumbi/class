#pragma once

#ifndef MUMBI__UTIL__SCOPED__H
#define MUMBI__UTIL__SCOPED__H

#include <functional>

namespace mumbi {
namespace util
{
	using std::function;	
	using std::forward;
	
	class scoped
	{
		using function_type = function<void(void)>;

		scoped(const scoped&) = delete;
		scoped& operator=(const scoped&) = delete;

	public:		
		scoped(function_type&& initializer, function_type&& finalizer)
			: _finalizer(forward<function_type>(finalizer))
		{
			initializer();
		}

		~scoped()
		{
			_finalizer();
		}

	private:
		function_type	_finalizer;
	};
}}

#endif	// MUMBI__UTIL__SCOPED__H