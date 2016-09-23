// reference. https://testbit.eu/cpp11-signal-system-performance/
#pragma once

#ifndef SIGNAL__EVENT_SIGNAL__H
#define SIGNAL__EVENT_SIGNAL__H

#include <functional>
#include <mutex>
#include <vector>
#include <cassert>

namespace mumbi {
namespace signal
{
	using std::function;	
	using std::vector;

	namespace lib
	{
		using std::mutex;
		using std::lock_guard;

		/// proto_signal is the template implementation for callback list.
		template<typename, typename> class proto_signal;   // undefined

		/// collector_invocation invokes signal handlers differently depending on return type.
		template<typename, typename> class collector_invocation;

		/// collector_base is base class of collector that has result as a member.
		template<typename EmissionResult>
		struct collector_base
		{
			using result_type = EmissionResult;			

			explicit collector_base() : _result() {}
			result_type result() { return _result; }

		protected:
			result_type	_result;
		};

		/// collector_last returns the result of the last signal handler from a signal emission.
		template<typename CallbackResult>
		struct collector_last : collector_base<CallbackResult>
		{			
			using callback_result_type = CallbackResult;

			bool operator() (const callback_result_type& callback_result)
			{
				_result = callback_result;
				return true;
			}
		};

		/// collector_default implements the default signal handler collection behaviour.
		template<typename CallbackResult>
		struct collector_default : collector_last<CallbackResult> {};

		/// collector_default specialisation for signals with void return type.
		template<>
		struct collector_default<void>
		{
			using result_type	= void;
			using element_type	= void;

			void result() {}
			inline bool operator() (void) { return true; }
		};

		/// collector_invocation specialisation for regular signals.
		template<class Collector, class R, class... Args>
		struct collector_invocation<Collector, R(Args...)>
		{
			inline bool invoke(Collector& collector, const function<R(Args...)>& callback_function, Args... args)
			{
				return collector(callback_function(args...));
			}
		};

		/// collector_invocation specialisation for signals with void return type.
		template<class Collector, class... Args>
		struct collector_invocation<Collector, void(Args...)>
		{
			inline bool invoke(Collector& collector, const function<void(Args...)>& callback_function, Args... args)
			{
				callback_function(args...);
				return collector();
			}
		};

		/// proto_signal template specialised for the callback signature and collector.
		template<class Collector, class R, class... Args>
		class proto_signal<R(Args...), Collector> : private collector_invocation<Collector, R(Args...)>
		{
		protected:			
			using callback_type				= function<R(Args...)>;
			using callback_result_type		= typename callback_type::result_type;
			using collector_type			= Collector;
			using collector_result_type		= typename collector_type::result_type;
			using lock_type					= mutex;

		private:
			/// signal_link implements a doubly-linked ring with ref-counted nodes containing the signal handlers.
			struct signal_link
			{
				signal_link* _next;
				signal_link* _prev;
				callback_type _callback;
				int _ref_count;

				explicit signal_link(const callback_type& callback) : _next(nullptr), _prev(nullptr), _callback(callback), _ref_count(1) {}
				/*dtor*/ ~signal_link() { assert(_ref_count == 0); }

				void incref() { _ref_count += 1; assert(_ref_count > 0); }
				void decref() { _ref_count -= 1; if (0 == _ref_count) delete this; else assert(_ref_count > 0); }

				void unlink()
				{
					_callback = nullptr;

					if (nullptr != _next)
						_next->_prev = _prev;

					if (nullptr != _prev)
						_prev->_next = _next;

					decref();
					// leave intact ->_next, ->_prev for stale iterators
				}

				size_t add_before(const callback_type& callback)
				{
					signal_link* link = new signal_link(callback);
					link->_prev = _prev; // link to last
					link->_next = this;
					_prev->_next = link; // link from last
					_prev = link;

					static_assert (sizeof(link) == sizeof(size_t), "sizeof size_t");
					return size_t(link);
				}

				bool deactivate(const callback_type& callback)
				{
					if (callback == _callback)
					{
						_callback = nullptr;      // deactivate static head
						return true;
					}

					for (signal_link *link = this->_next ? this->_next : this; link != this; link = link->_next)
					{
						if (callback == link->_callback)
						{
							link->unlink();     // deactivate and unlink sibling
							return true;
						}
					}

					return false;
				}

				bool remove_sibling(size_t id)
				{
					for (signal_link *link = this->_next ? this->_next : this; link != this; link = link->_next)
					{
						if (id == size_t(link))
						{
							link->unlink();     // deactivate and unlink sibling
							return true;
						}
					}

					return false;
				}
			};

			signal_link* _callback_ring; // linked ring of callback nodes
			lock_type	 _lock;

			/*copy-ctor*/ proto_signal(const proto_signal&) = delete;
			proto_signal& operator= (const proto_signal&) = delete;

			void ensure_ring()
			{
				if (nullptr == _callback_ring)
				{
					_callback_ring = new signal_link(callback_type()); // ref_count = 1
					_callback_ring->incref(); // ref_count = 2, head of ring, can be deactivated but not removed
					_callback_ring->_next = _callback_ring; // ring head initialization
					_callback_ring->_prev = _callback_ring; // ring tail initialization
				}
			}

		public:
			/// proto_signal constructor, connects default callback if non-NULL.
			proto_signal(const callback_type& callback)
				: _callback_ring(nullptr)
			{
				lock_guard<lock_type>	guard(_lock);

				if (callback)
				{
					ensure_ring();
					_callback_ring->_callback = callback;
				}
			}

			/// proto_signal destructor releases all resources associated with this signal.
			~proto_signal()
			{
				clear();
			}

			/// Operator to add a new function or lambda as signal handler, returns a handler connection ID.
			size_t operator+= (const callback_type& callback)
			{
				lock_guard<lock_type>	guard(_lock);

				ensure_ring();
				return _callback_ring->add_before(callback);
			}
			/// Operator to remove a signal handler through it connection ID, returns if a handler was removed.
			bool operator-= (size_t connection)
			{
				lock_guard<lock_type>	guard(_lock);

				return _callback_ring ? _callback_ring->remove_sibling(connection) : false;
			}

			void clear()
			{
				lock_guard<lock_type>	guard(_lock);

				if (nullptr != _callback_ring)
				{
					while (_callback_ring->_next != _callback_ring)
						_callback_ring->_next->unlink();

					assert(_callback_ring->_ref_count >= 2);
					_callback_ring->decref();
					_callback_ring->decref();

					_callback_ring = nullptr;
				}
			}

			/// Emit a signal, i.e. invoke all its callbacks and collect return types with the Collector.
			collector_result_type emit(Args... args)
			{	
				lock_guard<lock_type>	guard(_lock);

				collector_type collector;
				if (nullptr == _callback_ring)
					return collector.result();

				signal_link* link = _callback_ring;
				link->incref();

				do
				{
					if (nullptr != link->_callback)
					{
						const bool continue_emission = this->invoke(collector, link->_callback, args...);
						if (!continue_emission)
							break;
					}

					signal_link* old = link;
					link = old->_next;
					link->incref();
					old->decref();
				} while (link != _callback_ring);

				link->decref();
				return collector.result();
			}
		};

		/// protected_signal is the child of protected proto_signal.
		template<class, typename, class> class protected_signal;	// undefined.

																	/// protected_signal template specialised for the callback signature and collector.
		template<class Friend, typename R, typename... Args, class Collector>
		struct protected_signal<Friend, R(Args...), Collector> : protected proto_signal<R(Args...), Collector>
		{
			using proto_signal		= proto_signal<R(Args...), Collector>;
			using callback_type		= typename proto_signal::callback_type;

			friend typename Friend;

			/// Signal constructor, supports a default callback as argument.
			protected_signal(const callback_type& callback = callback_type()) : proto_signal(callback) {}
		};

	} // lib

	// namespace signal
	/**
	* Signal is a template type providing an interface for arbitrary callback lists.
	* A signal type needs to be declared with the function signature of its callbacks,
	* and optionally a return result collector class type.
	* Signal callbacks can be added with operator+= to a signal and removed with operator-=, using
	* a callback connection ID return by operator+= as argument.
	* The callbacks of a signal are invoked with the emit() method and arguments according to the signature.
	* The result returned by emit() depends on the signal collector class. By default, the result of
	* the last callback is returned from emit(). Collectors can be implemented to accumulate callback
	* results or to halt a running emissions in correspondance to callback results.
	* The signal implementation is safe against recursion, so callbacks may be removed and
	* added during a signal emission and recursive emit() calls are also safe.
	* The overhead of an unused signal is intentionally kept very low, around the size of a single pointer.
	* Note that the Signal template types is non-copyable.
	*/
	template <typename SignalSignature, class Collector = lib::collector_default<typename function<SignalSignature>::result_type>>
	struct signal final
		: public lib::proto_signal<SignalSignature, Collector>
	{
		using proto_signal	= lib::proto_signal<SignalSignature, Collector>;
		using callback_type	= typename proto_signal::callback_type;

		/// Signal constructor, supports a default callback as argument.
		signal(const callback_type& callback = callback_type()) : proto_signal(callback) {}
	};	

	/// event_signal is not public emission.
	template<class Friend, typename Signature, class Collector = lib::collector_default<typename function<Signature>::result_type>>
	struct event_signal final
		: public lib::protected_signal<Friend, Signature, Collector>
	{
		using protected_signal	= lib::protected_signal<Friend, Signature, Collector>;
		using callback_type		= typename protected_signal::callback_type;

		size_t operator+=(callback_type listener)
		{
			return protected_signal::operator+=(listener);
		}

		bool operator-=(size_t connection)
		{
			return protected_signal::operator-=(connection);
		}

		void clear()
		{
			protected_signal::clear();
		}
	};

	/// This function creates a std::function by binding @a object to the member function pointer @a method.
	template<class Instance, class Class, class R, class... Args> function<R(Args...)>
	slot(Instance& object, R(Class::*method) (Args...))
	{
		return [&object, method](Args... args) { return (object.*method) (args...); };
	}

	/// This function creates a std::function by binding @a object to the member function pointer @a method.
	template<class Class, class R, class... Args> function<R(Args...)>
	slot(Class* object, R(Class::*method) (Args...))
	{
		return [object, method](Args... args) { return (object->*method) (args...); };
	}

	template<typename CallbackResult, bool Continue>
	struct collector_continue : lib::collector_base<CallbackResult>
	{	
		using callback_result_type = CallbackResult;

		static constexpr bool continue_value = Continue;

		inline bool operator() (const callback_result_type& callback_result)
		{
			_result = callback_result;
			return _result ? continue_value : !continue_value;
		}
	};

	/// Keep signal emissions going while all handlers return !0 (true).
	template<typename Result>
	struct collector_until_0 : collector_continue<Result, true> {};	

	/// Keep signal emissions going while all handlers return 0 (false).
	template<typename Result>
	struct collector_while_0 : collector_continue<Result, false> {};

	/// CollectorVector returns the result of the all signal handlers from a signal emission in a std::vector.
	template<typename CallbackResult>
	struct collector_vector : lib::collector_base<vector<CallbackResult>>
	{	
		using callback_result_type = CallbackResult;

		inline bool operator() (const callback_result_type& callback_result)
		{
			_result.push_back(callback_result);
			return true;
		}
	};
}}

#endif	// SIGNAL__EVENT_SIGNAL__H