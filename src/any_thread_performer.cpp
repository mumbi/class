#include "any_thread_performer.h"

#include <mutex>
#include <atomic>
#include <thread>

#include "performable_service.h"

namespace mumbi {
namespace threading
{
	using std::chrono::seconds;

	class any_thread_performer::impl
	{
		friend class any_thread_performer;
		
		using lock_type	= mutex;

		impl()
		{
			start();
		}

		size_t post_count() const
		{
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
			return _performable_services.size() * thread::hardware_concurrency();			
		}

		bool post(performable_interface_ptr performable)
		{
			if (_stopped)
				return false;

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

			return true;
		}		

		void start()
		{
			lock_guard<impl::lock_type> guard(_performable_services_lock);

			if (_performable_services.empty())
				add_performable_service();

			_stopped = false;
		}

		void stop()
		{
			_stopped = true;

			lock_guard<impl::lock_type> guard(_performable_services_lock);

			for (auto& service_value : _performable_services)
			{
				performable_service& service = *service_value;

				service.stop();
			}

			_performable_services.clear();
		}

		performable_service& add_performable_service()
		{	
			_performable_services.emplace_back(new performable_service);
			return *_performable_services.back();
		}
		
		using performable_service_container = vector<unique_ptr<performable_service>>;

		lock_type						_performable_services_lock;
		performable_service_container	_performable_services;

		atomic<bool>					_stopped;
	};

	any_thread_performer::~any_thread_performer()
	{	
	}

	any_thread_performer::any_thread_performer()
		: _pimpl(new impl)
	{
	}

	size_t any_thread_performer::post_count() const
	{
		return _pimpl->post_count();
	}

	size_t any_thread_performer::working_count() const
	{
		return _pimpl->working_count();
	}

	size_t any_thread_performer::thread_count() const
	{
		return _pimpl->thread_count();
	}

	bool any_thread_performer::stopped() const
	{
		return _pimpl->_stopped;
	}

	void any_thread_performer::start()
	{
		_pimpl->start();		
	}

	void any_thread_performer::stop()
	{
		_pimpl->stop();		
	}

	bool any_thread_performer::post(performable_interface_ptr performable)
	{
		return _pimpl->post(performable);
	}
}}