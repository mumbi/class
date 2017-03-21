#pragma once

#ifndef MUMBI__THREADING__SPIN_LOCK__H
#define MUMBI__THREADING__SPIN_LOCK__H

#include <atomic>

namespace mumbi {
namespace threading
{
	using std::atomic;
	using std::memory_order_acquire;
	using std::memory_order_release;

	class spin_lock
	{
	public:
		spin_lock()
			: _is_locked(false)
		{
		}

		inline bool is_locked() const
		{
			return _is_locked;
		}

		inline void lock()
		{
			bool is_locked = false;
			while (!_is_locked.compare_exchange_weak(is_locked, true, memory_order_acquire))
			{
				is_locked = false;
			}
		}

		inline void unlock()
		{	
			_is_locked.store(false, memory_order_release);
		}

	private:
		atomic<bool>	_is_locked;
	};
}}

#endif	// MUMBI__THREADING__SPIN_LOCK__H
