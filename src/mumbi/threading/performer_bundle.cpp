#include "performer_bundle.h"

#include <vector>
#include <unordered_map>
#include <atomic>
#include <mutex>

#include "common_performer.h"
#include "dedicated_performer.h"
#include "performer.h"

namespace mumbi {
namespace threading
{
	using std::unique_ptr;
	using std::vector;
	using std::unordered_map;
	using std::atomic;
	using std::mutex;
	using std::lock_guard;
	using std::thread;
	using std::condition_variable;
	using std::unique_lock;
	using std::make_unique;
	using std::bind;

	using lock_type = mutex;

	class performer_bundle::impl
	{
		friend class async_performer;
	public:
		class dedicated_performing_thread
			: public performer
		{
		public:
			size_t post_count() const
			{
				return _performer.post_count();
			}

			bool stopped() const
			{
				return _performer.stopped();
			}

			void restart()
			{
				_performer.restart();				
			}

			void stop()
			{
				_performer.stop();
			}

			void start_running()
			{
				_thread = make_unique<thread>(bind(&dedicated_performer::run, &_performer));
			}

			void end_running()
			{
				if (_thread)
				{
					if (_thread->joinable())
						_thread->join();
				}
			}

			dedicated_performer& get_dedicated_performer()
			{
				return _performer;
			}
			
			void post(const performable& performable) override
			{
				_performer.post(performable);
			}			

		private:
			dedicated_performer		_performer;
			unique_ptr<thread>		_thread;
		};
		
		using dedicated_performing_thread_type	= unordered_map<int, dedicated_performing_thread>;

		impl()
			: _stopped(true)
		{	
		}

		size_t dedicated_performing_thread_count() const
		{
			lock_guard<lock_type> lock(_dedicated_lock);

			return _dedicated_performing_threads.size();
		}

		void restart_dedicated_performing_threads()
		{
			lock_guard<lock_type> lock(_dedicated_lock);

			for (auto& value : _dedicated_performing_threads)
			{
				auto& dedicated_performing_thread = value.second;
				dedicated_performing_thread.restart();
			}
		}

		void stop_dedicated_performing_threads()
		{
			lock_guard<lock_type> lock(_dedicated_lock);

			for (auto& value : _dedicated_performing_threads)
			{
				auto& dedicated_performing_thread = value.second;
				dedicated_performing_thread.stop();
			}
		}
		
		/*void post_dedicated_performing_threads(int id, performable& performable)
		{
			lock_guard<lock_type> lock(_dedicated_lock);

			dedicated_performing_thread& dedicated_performing_thread = _dedicated_performing_threads[id];
			dedicated_performing_thread.post(performable);

			if (!_stopped && dedicated_performing_thread.stopped())
				dedicated_performing_thread.restart();
		}*/

		dedicated_performer& get_dedicated_performer(int id)
		{
			lock_guard<lock_type> lock(_dedicated_lock);

			dedicated_performing_thread& dedicated_performing_thread = _dedicated_performing_threads[id];

			if (!_stopped && dedicated_performing_thread.stopped())
				dedicated_performing_thread.restart();

			return dedicated_performing_thread.get_dedicated_performer();
		}

		void start_running_dedicated_performing_threads()
		{
			lock_guard<lock_type> lock(_dedicated_lock);

			for (auto& value : _dedicated_performing_threads)
			{
				auto& dedicated_performing_thread = value.second;
				dedicated_performing_thread.start_running();
			}
		}

		void end_running_dedicated_performing_threads()
		{
			lock_guard<lock_type> lock(_dedicated_lock);

			for (auto& value : _dedicated_performing_threads)
			{
				auto& dedicated_performing_thread = value.second;
				dedicated_performing_thread.end_running();
			}
		}

		common_performer					_common_performer;
		dedicated_performing_thread_type	_dedicated_performing_threads;
		dedicated_performer					_running_thread_performer;

		atomic<bool>						_stopped;

		mutable lock_type					_dedicated_lock;
	};

	performer_bundle::~performer_bundle()
	{
		stop();		
	}

	performer_bundle::performer_bundle()
		: _pimpl(make_unique<impl>())
	{	
	}	

	bool performer_bundle::stopped() const
	{
		return _pimpl->_stopped;
	}

	void performer_bundle::restart()
	{
		_pimpl->_common_performer.restart();		
		_pimpl->restart_dedicated_performing_threads();
		_pimpl->_running_thread_performer.restart();
		
		_pimpl->_stopped = false;
	}

	void performer_bundle::stop()
	{
		_pimpl->_common_performer.stop();		
		_pimpl->stop_dedicated_performing_threads();
		_pimpl->_running_thread_performer.stop();

		_pimpl->_stopped = true;		
	}
	
	void performer_bundle::run()
	{
		_pimpl->_common_performer.start_running();
		_pimpl->start_running_dedicated_performing_threads();

		_pimpl->_running_thread_performer.run();

		_pimpl->_common_performer.end_running();
		_pimpl->end_running_dedicated_performing_threads();
	}
	
	void performer_bundle::post(const performable& performable)
	{
		_pimpl->_common_performer.post(performable);
	}

	size_t performer_bundle::identical_performer_count() const
	{
		return _pimpl->dedicated_performing_thread_count();
	}

	dedicated_performer& performer_bundle::get_identical_performer(int id)
	{
		return _pimpl->get_dedicated_performer(id);
	}

	dedicated_performer& performer_bundle::get_running_thread_performer()
	{
		return _pimpl->_running_thread_performer;
	}
	
	/*void performer_bundle::post_on_running(performable& performable)
	{
		_pimpl->_running_thread_performer.post(performable);
	}	
	
	void performer_bundle::post(int id, performable& performable)
	{
		_pimpl->post_dedicated_performing_threads(id, performable);
	}	

	size_t performer_bundle::dedicated_performer_count() const
	{
		return _pimpl->dedicated_performing_thread_count();
	}	*/
}}