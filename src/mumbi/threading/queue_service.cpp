#include "queue_service.h"

namespace mumbi {
namespace threading
{
	using std::make_unique;
	using std::lock_guard;	

	queue_service::~queue_service()
	{
	}	

	queue_service::queue_service()
		: _work(make_unique<io_service::work>(_io_service))
	{		
	}

	queue_service::queue_service(size_t concurrency_hint)
		: _io_service(concurrency_hint)
		, _work(make_unique<io_service::work>(_io_service))
	{		
	}

	bool queue_service::stopped() const
	{
		return _io_service.stopped();
	}

	void queue_service::restart()
	{
		if (!stopped())
			return;

		lock_guard<mutex> lock(_lock);

		_io_service.restart();
		_work = make_unique<io_service::work>(_io_service);
	}

	void queue_service::stop()
	{
		if (stopped())
			return;

		lock_guard<mutex> lock(_lock);

		_work = nullptr;
		_io_service.stop();		
	}

	void queue_service::run()
	{
		_io_service.run();
	}

	io_service& queue_service::get_io_service()
	{
		return _io_service;
	}
}}