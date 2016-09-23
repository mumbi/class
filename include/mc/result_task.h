#pragma once

#ifndef RESULT_TASK__H
#define RESULT_TASK__H

#include "task.h"
#include <functional>

namespace mumbi {
namespace threading {
namespace task
{
	using std::forward;
	using std::function;
	using std::bind;

	template<class T>
	class base_result_task
		: public task<void>
	{
	public:
		using result_type = T;
		using function_type = function<result_type(void)>;
		using result_function_type = function<void(result_type)>;

	protected:
		using internal_execute_function = function<void(void)>;

	protected:
		base_result_task(function_type&& f, result_function_type&& result, internal_execute_function&& execute)
			: task(forward<internal_execute_function>(execute))
			, _function(forward<function_type>(f))
			, _result_function(forward<result_function_type>(result))
		{
		}

	protected:
		function_type			_function;
		result_function_type	_result_function;
	};

	template<class T>
	class result_task		
		: public base_result_task<T>
	{
	public:
		result_task(function_type&& f, result_function_type&& result)
			: base_result_task(forward<function_type>(f), forward<result_function_type>(result), bind(&result_task::execute, this))			
		{
		}

	private:
		void execute()
		{
			if (_function)
			{
				result_type result = _function();

				if (_result_function)
					_result_function(result);
			}
		}	
	};

	template<>
	class result_task<void>		
		: public base_result_task<void>
	{
	public:
		result_task(function_type&& f, result_function_type&& result)
			: base_result_task(forward<function_type>(f), forward<result_function_type>(result), bind(&result_task::execute, this))			
		{
		}

	private:
		void execute()
		{
			if (_function)
			{
				_function();

				if (_result_function)
					_result_function();
			}
		}	
	};
}}}

#endif	// RESULT_TASK__H
