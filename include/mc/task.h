#pragma once

#ifndef TASK__H
#define TASK__H

#include "performable_interface.h"

#include <future>

namespace mumbi {
namespace threading {
namespace task
{
	using std::promise;
	using std::future;
	using std::function;	
	using std::exception;

	template<typename R>
	struct base_invoker
	{
		using return_type = R;

		template<typename F> base_invoker(F&& f) : _function(forward<F>(f)) {}

		promise<return_type>& get_promise() { return _promise; }		

		void invoke()
		{
			if (_function)
			{
				try
				{
					_function();
				}
				catch (exception& e)
				{
					auto exception = make_exception_ptr(e);
					_promise.set_exception(exception);

					rethrow_exception(exception);
				}
			}
		}

		function<return_type(void)>	_function;
		promise<return_type>		_promise;
	};

	template<typename R>
	struct invoker
		: public base_invoker<R>
	{
		template<typename F> 
		invoker(F&& f)
			: base_invoker(forward<F>(f))
			, _return_value()
		{
		}
	};

	template<>
	struct invoker<void>
		: public base_invoker<void>
	{
		template<typename F>
		invoker(F&& f)
			: base_invoker(forward<F>(f))
		{
		}
	};

	template<typename R>
	class task
		: public performable_interface
	{
		using return_type = R;

	public:
		template<typename F>
		task(F&& f)
			: _invoker(forward<F>(f))
		{
		}

		void perform() override
		{
			_invoker.invoke();		
		}

		future<return_type> get_future()
		{
			return _invoker.get_promise().get_future();
		}

	protected:
		promise<return_type>& get_promise()
		{
			return _invoker.get_promise();
		}

	private:
		invoker<return_type>	_invoker;
	};	

	template<typename F>
	auto make_task(F&& f) -> task<decltype(f())>
	{
		return task(forward<F>(f));
	}

	template<typename F>
	auto make_shared_task(F&& f)
	{
		return make_shared<decltype(make_task(f))>(forward<F>(f));
	}
}}}

#endif	// TASK__H
