#include "specific_thread_performer.h"

#include <functional>

#include "performable_queue.h"

namespace mumbi {
namespace threading
{
	class specific_thread_performer::impl
	{
		friend class specific_thread_performer;

		size_t post_count() const
		{
			return _performable_queue.post_count();
		}
		
		bool post(performable_interface_ptr performable)
		{
			if (_performable_queue.stopped())
				return false;

			_performable_queue.post(performable);

			return true;
		}

		void run()
		{
			_performable_queue.run();			
		}

		performable_queue	_performable_queue;
	};

	specific_thread_performer::~specific_thread_performer()
	{
		_pimpl->_performable_queue.stop();
	}

	specific_thread_performer::specific_thread_performer()
		: _pimpl(new impl)
	{
		start();
	}

	size_t specific_thread_performer::post_count() const
	{
		return _pimpl->post_count();
	}

	bool specific_thread_performer::stopped() const
	{
		return _pimpl->_performable_queue.stopped();
	}

	void specific_thread_performer::start()
	{
		_pimpl->_performable_queue.start();
	}

	void specific_thread_performer::stop()
	{
		_pimpl->_performable_queue.stop();
	}

	bool specific_thread_performer::post(performable_interface_ptr performable)
	{
		return _pimpl->post(performable);
	}

	void specific_thread_performer::run()
	{
		_pimpl->run();
	}	
}}

