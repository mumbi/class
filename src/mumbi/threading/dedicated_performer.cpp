#include "dedicated_performer.h"

#include <functional>

#include "performable_queue.h"

namespace mumbi {
namespace threading
{
	using std::make_unique;

	class dedicated_performer::impl
	{
		friend class dedicated_performer;

	public:
		performable_queue	_performable_queue;
	};

	dedicated_performer::~dedicated_performer()
	{
		stop();
	}

	dedicated_performer::dedicated_performer()
		: _pimpl(make_unique<impl>())
	{	
	}

	size_t dedicated_performer::post_count() const
	{
		return _pimpl->_performable_queue.post_count();
	}

	bool dedicated_performer::stopped() const
	{
		return _pimpl->_performable_queue.stopped();
	}

	void dedicated_performer::restart()
	{
		_pimpl->_performable_queue.restart();
	}

	void dedicated_performer::stop()
	{
		_pimpl->_performable_queue.stop();
	}

	void dedicated_performer::run()
	{
		_pimpl->_performable_queue.run();
	}
	
	void dedicated_performer::post(const performable& performable)
	{
		_pimpl->_performable_queue.post(performable);
	}	
}}

