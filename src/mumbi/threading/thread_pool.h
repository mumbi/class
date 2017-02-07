#pragma once

#ifndef MUMBI__THREADING__THREAD_POOL__H
#define MUMBI__THREADING__THREAD_POOL__H

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

	class thread_pool final
	{
		using function_type = function<void()>;		

	public:
		~thread_pool()
		{
			join();
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
				_threads.emplace_back(_function);
			}
		}
		
		void join()
		{			
			for (auto& t : _threads)
			{
				if (t.joinable())					
					t.join();
			}

			_threads.clear();
		}		

		size_t thread_count() const
		{
			return _threads.size();
		}		

	private:		
		function_type		_function;

		const size_t		_max_thread_count;
		vector<thread>		_threads;		
	};
}}

#endif	// MUMBI__THREADING__THREAD_POOL__H