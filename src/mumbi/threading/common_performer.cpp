#include "common_performer.h"

#include <mutex>
#include <atomic>
#include <thread>

#include "performable_service.h"

namespace mumbi {
namespace threading
{
	using std::chrono::seconds;
	using std::unique_lock;
	using std::condition_variable;
	using std::make_unique;

	class common_performer::impl
	{
		friend class common_performer;

	public:		
		using lock_type	= mutex;

		impl()
			: _stopped(false)
			, _running(false)
		{
		}

		size_t post_count() const
		{
			lock_guard<lock_type> guard(_performable_services_lock);

			size_t post_count = 0;
			for (auto& service_value : _performable_services)
			{
				performable_service& service = *service_value;

				post_count += service.post_count();
			}

			return post_count;
		}

		size_t working_count() const
		{
			lock_guard<lock_type> guard(_performable_services_lock);

			size_t working_count = 0;
			for (auto& service_value : _performable_services)
			{
				performable_service& service = *service_value;

				working_count += service.working_count();
			}

			return working_count;
		}

		size_t thread_count() const
		{
			lock_guard<lock_type> guard(_performable_services_lock);

			size_t thread_count = 0;
			for (auto& service_value : _performable_services)
			{
				performable_service& service = *service_value;

				thread_count += service.thread_count();
			}

			return thread_count;
		}
		
		void post(performable& performable)
		{
			lock_guard<lock_type> guard(_performable_services_lock);

			bool posted = false;
			for (auto service_iterator = _performable_services.begin(); _performable_services.end() != service_iterator; )
			{
				performable_service& service = *(*service_iterator);
				if (false == posted)
				{
					if (false == service.busy())
					{
						service.post(performable);
						posted = true;
					}
				}
				else
				{
					if (service.post_count() <= 0 &&
						performable_service::clock_type::now() - service.idle_start_time() > seconds(5))
					{
						service_iterator = _performable_services.erase(service_iterator);
						continue;
					}
				}

				++service_iterator;
			}

			// new service
			if (false == posted)
			{
				performable_service& service = add_performable_service();
				service.post(performable);
			}
		}		

		void restart()
		{
			lock_guard<impl::lock_type> guard(_performable_services_lock);

			if (_performable_services.empty())
				add_performable_service();

			_stopped = false;
		}

		void stop()
		{
			lock_guard<impl::lock_type> guard(_performable_services_lock);

			for (auto& service_value : _performable_services)
			{
				performable_service& service = *service_value;

				service.stop();
			}

			_stopped = true;
			_stopped_condition.notify_all();
		}

		void start_running()
		{			
			lock_guard<impl::lock_type> guard(_performable_services_lock);

			for (auto& service_value : _performable_services)
			{
				performable_service& service = *service_value;

				service.start_running();
			}

			_running = true;
		}

		void end_running()
		{
			// wait to stop.
			{
				unique_lock<mutex>	lock(_stopped_lock);
				_stopped_condition.wait(lock, [this]() { return _stopped.load(); });
			}			

			// end running.
			{
				lock_guard<impl::lock_type> guard(_performable_services_lock);

				for (auto& service_value : _performable_services)
				{
					performable_service& service = *service_value;

					service.end_running();
				}

				_performable_services.clear();

				_running = false;
			}
		}		

		performable_service& add_performable_service()
		{	
			_performable_services.emplace_back(make_unique<performable_service>());
			performable_service& service = *_performable_services.back();

			if (_running)
				service.start_running();

			return service;
		}
		
		using performable_service_container = vector<unique_ptr<performable_service>>;

		mutable lock_type				_performable_services_lock;
		performable_service_container	_performable_services;

		atomic<bool>					_running;

		atomic<bool>					_stopped;
		mutex							_stopped_lock;
		condition_variable				_stopped_condition;
	};

	common_performer::~common_performer()
	{
		stop();

		end_running();		
	}

	common_performer::common_performer()
		: _pimpl(make_unique<impl>())
	{
	}

	size_t common_performer::post_count() const
	{
		return _pimpl->post_count();
	}

	size_t common_performer::working_count() const
	{
		return _pimpl->working_count();
	}

	size_t common_performer::thread_count() const
	{
		return _pimpl->thread_count();
	}

	bool common_performer::stopped() const
	{
		return _pimpl->_stopped;
	}

	void common_performer::restart()
	{
		_pimpl->restart();		
	}

	void common_performer::stop()
	{
		_pimpl->stop();		
	}

	void common_performer::start_running()
	{
		_pimpl->start_running();
	}

	void common_performer::end_running()
	{
		_pimpl->end_running();
	}
	
	void common_performer::post(performable& performable)
	{
		_pimpl->post(performable);
	}
}}