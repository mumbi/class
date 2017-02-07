#pragma once

#ifndef MUMBI__THREADING__PROMISE__H
#define MUMBI__THREADING__PROMISE__H

#include "../std/experimental/any.h"
#include "performable.h"
#include "performer.h"
#include "tasking/task.h"

#include "../util/traits.h"
#include "../util/tuple.h"

namespace mumbi {
namespace threading
{
	using std::unique_ptr;
	using std::mutex;
	using std::vector;
	using std::forward;
	using std::make_shared;	
	using std::bind;
	using std::lock_guard;
	using std::make_tuple;
	using std::exception;
	using std::experimental::any;
	using std::string;
	using std::exception_ptr;
	using std::runtime_error;
	using std::is_bind_expression;
	using std::current_exception;
	using std::bad_cast;	
	
	using mumbi::threading::tasking::task_invoker;	
	using mumbi::util::function_traits;
	using mumbi::util::transform;

	class precedence_promise_exception
		: public runtime_error
	{
	public:
		explicit precedence_promise_exception(const string& message, exception_ptr exception)
			: runtime_error(message)
			, _exception(exception)
		{
		}

		explicit precedence_promise_exception(const char* message, exception_ptr exception)
			: runtime_error(message)
			, _exception(exception)
		{
		}

		exception_ptr exception() const
		{
			return _exception;
		}

	private:
		exception_ptr	_exception;
	};

	template<typename, typename>
	class base_promise_task;

	template<typename Promise, typename R, typename... Args>
	class base_promise_task<Promise, R(Args...)>
		: public performable
	{
	protected:
		class base_promise_invoker
			: public task_invoker<R(Args...)>
		{
		public:
			template<typename Callable>
			base_promise_invoker(performer& performer, Callable&& callable)
				: task_invoker(forward<Callable>(callable))
				, _done(false)
				, _performer(performer)
			{
			}			

			virtual void post_with_arguments(Promise& promise) = 0;

			void after_invoked() override
			{
				post_promises();
			}

			void post_promises()
			{
				lock_guard<mutex> guard(_lock);

				for (auto& promise : _promises)
				{
					post_with_arguments(promise);
				}

				_done = true;
			}

			template<typename Callable>
			auto then(performer& performer, Callable&& callable)
			{
				lock_guard<mutex> guard(_lock);

				auto p = promise<function_traits<Callable>::function_type>(performer, forward<Callable>(callable), false);

				if (_done)
				{
					post_with_arguments(p);
				}
				else
				{
					_promises.push_back(p);
				}

				return p;
			}

			void post(performable& performable)
			{
				_performer.post(performable);
			}

		private:
			mutex			_lock;
			bool			_done;
			performer&		_performer;

			vector<Promise>	_promises;
		};		

	public:
		template<typename Callable>
		auto then(performer& performer, Callable&& callable)
		{
			auto invoker = get_invoker<base_promise_invoker>();
			return invoker->then(performer, forward<Callable>(callable));
		}		

		R result()
		{
			auto invoker = get_invoker<base_promise_invoker>();
			return invoker->result();
		}

		void post(Args&&... args)
		{
			auto invoker = get_invoker<base_promise_invoker>();
			invoker->set_arguments(forward<Args>(args)...);
			invoker->post(*this);
		}

		void set_exception(exception_ptr exception)
		{
			auto invoker = get_invoker<base_promise_invoker>();
			return invoker->set_exception(exception);
		}

		void after_invoked()
		{
			auto invoker = get_invoker<base_promise_invoker>();
			return invoker->after_invoked();
		}

	protected:		
		base_promise_task(shared_ptr<base_promise_invoker>&& invoker)
			: performable(forward<shared_ptr<base_promise_invoker>>(invoker))
		{
		}
	};

	template<typename...>
	class args_promise;

	template<typename>
	class promise_task;

	template<typename R, typename... Args>
	class promise_task<R(Args...)>
		: public base_promise_task<args_promise<R>, R(Args...)>
	{
	public:
		using args_promise_type = args_promise<R>;		

		class promise_invoker
			: public base_promise_invoker
		{
		public:
			template<typename Callable>
			promise_invoker(performer& performer, Callable&& callable)
				: base_promise_invoker(performer, forward<Callable>(callable))
			{
			}			

			void post_with_arguments(args_promise_type& promise) override
			{
				try
				{
					R r = result();
					promise.post(forward<R>(r));
				}
				catch (...)
				{
					promise.set_exeption(make_exception_ptr(precedence_promise_exception("precedence promise exception", current_exception())));
					promise.after_invoked();
				}

			}
		};

	public:
		template<typename Callable>
		promise_task(performer& performer, Callable&& callable)			
			: base_promise_task(make_shared<promise_invoker>(performer, forward<Callable>(callable)))
		{
		}
	};

	template<typename... Args>
	class promise_task<void(Args...)>
		: public base_promise_task<args_promise<>, void(Args...)>
	{
	public:
		using args_promise_type = args_promise<>;		

		class promise_invoker
			: public base_promise_invoker
		{
		public:
			template<typename Callable>
			promise_invoker(performer& performer, Callable&& callable)
				: base_promise_invoker(performer, forward<Callable>(callable))
			{
			}

			void post_with_arguments(args_promise_type& promise) override
			{
				try
				{
					result();
					promise.post();
				}
				catch (...)
				{
					promise.set_exeption(make_exception_ptr(precedence_promise_exception("precedence promise exception", current_exception())));
					promise.after_invoked();
				}
			}
		};

	public:
		template<typename Callable>
		promise_task(performer& performer, Callable&& callable)			
			: base_promise_task(make_shared<promise_invoker>(performer, forward<Callable>(callable)))
		{
		}
	};

	template<typename... Args>
	class args_promise
	{
		template<typename>
		friend class promise_task;

	private:
		template<typename... ImplArgs>
		class base_impl
		{
		public:
			virtual void post(ImplArgs&&... args) = 0;
			virtual void set_exception(exception_ptr exception) = 0;
			virtual void after_invoked() = 0;
		};

		template<typename>
		class impl;

		template<typename R, typename... ImplArgs>
		class impl<R(ImplArgs...)>
			: public base_impl<ImplArgs...>
		{
		public:
			using result_type = R;
			using function_type = R(ImplArgs...);

		public:
			template<typename Callable>
			impl(performer& performer, Callable&& callable, bool post)
				: _promise_task(performer, forward<Callable>(callable))				
			{
				if (post)
					performer.post(_promise_task);
			}			

			result_type result()
			{
				return _promise_task.result();
			}

			void post(ImplArgs&&... args) override
			{
				_promise_task.post(forward<ImplArgs>(args)...);
			}

			void set_exception(exception_ptr exception) override
			{				
				_promise_task.set_exception(exception);
			}

			void after_invoked() override
			{
				return _promise_task.after_invoked();
			}

			template<typename Callable>
			auto then(performer& performer, Callable&& callable)
			{
				return _promise_task.then(performer, forward<Callable>(callable));
			}

		private:
			promise_task<function_type>	_promise_task;			
		};

		template<typename R>
		struct performer_holder
		{
			explicit performer_holder(performer& performer)
				: _performer(performer)
			{
			}

			performer&	_performer;
		};

	private:
		using base_impl_type = base_impl<Args...>;

	private:
		void post(Args&&... args)
		{
			_impl->post(forward<Args>(args)...);
		}

		void set_exeption(exception_ptr exception)
		{
			_impl->set_exception(exception);
		}

		void after_invoked()
		{
			_impl->after_invoked();
		}

	protected:
		template<typename R, typename Callable>
		args_promise(performer_holder<R>& performer_holder, Callable&& callable, bool post)
			: _impl(make_shared<impl<R(Args...)>>(performer_holder._performer, forward<Callable>(callable), post))
		{
		}

		template<typename T>
		shared_ptr<T> get_impl()
		{
			return static_pointer_cast<T>(_impl);
		}

	private:
		shared_ptr<base_impl_type>	_impl;
	};

	template<typename>
	class promise;

	template<typename R, typename... Args>
	class promise<R(Args...)>
		: public args_promise<Args...>
	{
		template<typename, typename>
		friend class base_promise_task;

	public:
		using result_type = R;
		using function_type = R(Args...);

	public:
		template<typename Callable>
		promise(performer& performer, Callable&& callable)
			: args_promise(performer_holder<result_type>(performer), forward<Callable>(callable), true)
		{
		}		

		result_type result()
		{
			auto implementation = get_impl<impl<function_type>>();
			return implementation->result();
		}

		result_type result() const
		{
			return const_cast<promise*>(this)->result();
		}

		template<typename Callable>
		auto then(performer& performer, Callable&& callable)
		{
			auto implementation = get_impl<impl<function_type>>();
			return implementation->then(performer, forward<Callable>(callable));
		}		

	private:
		template<typename Callable>
		promise(performer& performer, Callable&& callable, bool post)
			: args_promise(performer_holder<result_type>(performer), forward<Callable>(callable), post)
		{
		}
	};

	template<typename Callable>
	auto make_promise(performer& performer, Callable&& callable)
	{
		static_assert(false == is_bind_expression<Callable>::value, "bind object has to be wrapped by std::function.");

		return promise<function_traits<Callable>::function_type>(performer, forward<Callable>(callable));
	}	

	template<class Promise>
	auto waterfall_promise_then(performer& performer, Promise& promise)
	{
		return promise;
	}

	template<class Promise, typename Callable, typename... Callables>
	auto waterfall_promise_then(performer& performer, Promise& promise, Callable&& callable, Callables&&... callables)
	{
		return waterfall_promise_then(performer, promise.then(performer, forward<Callable>(callable)), forward<Callables>(callables)...);
	}
	
	template<typename Callable, typename... Callables>
	auto waterfall_promise(performer& performer, Callable&& callable, Callables&&... callables)
	{
		auto waterfall_promise = waterfall_promise_then(performer, make_promise(performer, forward<Callable>(callable)), forward<Callables>(callables)...);
		return waterfall_promise.result();		
	}

	template<typename... Callables>
	auto make_promises(performer& performer, Callables&&... callables)
	{
		return make_tuple(make_promise(performer, forward<Callables>(callables))...);
	}

	template<typename... Callables>
	auto parallel_promise(performer& performer, Callables&&... callables)
	{
		auto promises = make_promises(performer, forward<Callables>(callables)...);

		return make_promise(performer, [promises]()
		{
			return transform(promises, [](auto& promise)
			{
				try
				{
					return any(promise.result());
				}
				catch (...)
				{
					return any(current_exception());
				}					
			});			
		});
	}

	template<class Tuple>
	bool has_exception_in_any_tuple(const Tuple& result)
	{
		bool has_exception = false;
		for_each(result, [&has_exception](const any& a)
		{
			try
			{
				any_cast<exception_ptr>(a);

				has_exception = true;
			}
			catch (bad_cast&)
			{
			}
		});

		return has_exception;
	}
}}

#endif	// MUMBI__THREADING__PROMISE__H
