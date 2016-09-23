#pragma once

#ifndef THREAD_POOL__H
#define THREAD_POOL__H

#include <thread>
#include <vector>
#include <algorithm>
#include <mutex>
#include <atomic>

namespace mumbi {
namespace threading
{
	using std::thread;
	using std::atomic;
	using std::function;
	using std::vector;

	class thread_pool
	{
		using function_type = function<void()>;		

	public:
		~thread_pool()
		{	
		}

		thread_pool(const function_type& function)
			: _function(function)
			, _max_thread_count(thread::hardware_concurrency())
		{	
		}

		thread_pool(const function_type& function, size_t thread_count)
			: _function(function)
			, _max_thread_count(thread_count > 0 ? thread_count : thread::hardware_concurrency())
		{			
		}

		void start()
		{
			for (size_t i = 0; i < _max_thread_count; ++i)
			{
				_threads.emplace_back(&thread_pool::execution_on_thread, this);				
			}

			_thread_count = _max_thread_count;
		}

		void stop()
		{
			for_each(_threads.begin(), _threads.end(), [](auto& t)
			{
				if (t.joinable())					
					t.join();
			});

			_threads.clear();

			_thread_count = 0;
		}		

		size_t thread_count() const
		{
			return _thread_count;
		}

		void execution_on_thread()
		{
			if (_function)
				_function();
		}

	private:		
		function_type		_function;

		const size_t		_max_thread_count;
		vector<thread>		_threads;
		atomic<size_t>		_thread_count;
	};
}}

#endif	// THREAD_POOL__H