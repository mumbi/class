#pragma once

#ifndef MUMBI__THREADING__TASKING__TASK__H
#define MUMBI__THREADING__TASKING__TASK__H

#include "../performable.h"
#include "../invoker.h"

#include <future>
#include <functional>
#include <tuple>
#include <mutex>

#include "../../util/traits.h"

namespace mumbi {
namespace threading {
namespace tasking
{	
	using std::future;
	using std::shared_future;	
	using std::promise;
	using std::function;
	using std::current_exception;
	using std::future_status;
	using std::tuple;
	using std::index_sequence;
	using std::make_index_sequence;

	using std::make_shared;
	using std::forward;
	using std::shared_ptr;
	
	using std::mutex;
	using std::lock_guard;
	
	using std::exception_ptr;
	
	using mumbi::util::function_traits;	
	
	template<typename>
	class base_task_invoker;

	template<typename R, typename... Args>
	class base_task_invoker<R(Args...)>
		: public invoker
	{
	private:	
		using invoke_lock = mutex;

	public:
		using result_type = R;
		using function_type = R(Args...);

		template<typename Callable>
		base_task_invoker(Callable&& callable)
			: _callable(forward<Callable>(callable))
			, _future(_promise.get_future().share())
		{
		}

		virtual void after_invoked()
		{
		}

		result_type result()
		{			
			return _future.get();
		}

		void set_arguments(Args&&... args)
		{			
			lock_guard<invoke_lock>	lock(_invoke_lock);

			_arguments.swap(tuple<Args...>(forward<Args>(args)...));
		}

		void set_exception(exception_ptr exception)
		{
			_promise.set_exception(exception);
		}

		R result() const
		{
			return const_cast<base_task_invoker*>(this)->result();
		}

		template<size_t... Is>
		auto func(index_sequence<Is...>)
		{
			lock_guard<invoke_lock>	lock(_invoke_lock);

			return _callable(std::get<Is>(_arguments)...);
		}

		promise<result_type>		_promise;

		function<function_type>		_callable;

		tuple<Args...>				_arguments;
		invoke_lock					_invoke_lock;

		shared_future<result_type>	_future;
	};
	
	template<typename>
	class task_invoker;

	template<typename R, typename... Args>
	class task_invoker<R(Args...)>
		: public base_task_invoker<R(Args...)>		
	{		
	public:
		using result_type	= R;
		using function_type = R(Args...);	

	public:
		template<typename Callable>
		explicit task_invoker(Callable&& callable)	
			: base_task_invoker(forward<Callable>(callable))			
		{
		}
		
		bool invoke() override final
		{
			bool done = true;

			try
			{
				R return_value = func(make_index_sequence<sizeof...(Args)>{});
				_promise.set_value(return_value);
			}
			catch (...)
			{
				_promise.set_exception(current_exception());
				done = false;
			}

			after_invoked();

			return done;
		}

		task_invoker(task_invoker& other) = delete;
		task_invoker& operator=(task_invoker& other) = delete;
	};	

	template<typename... Args>
	class task_invoker<void(Args...)>
		: public base_task_invoker<void(Args...)>
	{
	public:
		using result_type = void;
		using function_type = void(Args...);

	public:
		template<typename Callable>
		explicit task_invoker(Callable&& callable)
			: base_task_invoker(forward<Callable>(callable))			
		{
		}

		bool invoke() override
		{
			bool done = true;

			try
			{
				func(make_index_sequence<sizeof...(Args)>{});
				_promise.set_value();
			}
			catch (...)
			{
				_promise.set_exception(current_exception());
				done = false;
			}

			after_invoked();

			return done;
		}

		task_invoker(task_invoker& other) = delete;
		task_invoker& operator=(task_invoker& other) = delete;
	};

	template<typename>
	class task;

	template<typename R, typename... Args>
	class task<R(Args...)> final
		: public performable
	{
	public:
		using return_type = R;
		using function_type = R(Args...);

	private:
			

	public:
		template<typename Callable>		
		explicit task(Callable&& callable)			
			: performable(make_shared<task_invoker<function_type>>(forward<Callable>(callable)))
		{
		}

		return_type result()
		{			
			auto invoker = get_invoker<base_task_invoker<function_type>>();
			return invoker->result();
		}

		return_type result() const
		{
			return const_cast<task*>(this)->result();
		}

		void set_arguments(Args&&... args)
		{
			auto invoker = get_invoker<base_task_invoker<function_type>>();
			invoker->set_arguments(forward<Args>(args)...);
		}
	};	

	template<typename Callable>
	auto make_task(Callable&& f)
	{
		return task<function_traits<Callable>::function_type>(forward<Callable>(f));
	}
}}}

#endif	// MUMBI__THREADING__TASKING__TASK__H
