#include "queue_service.h"

namespace mumbi
{
	using std::make_unique;

	queue_service::~queue_service()
	{
	}

	queue_service::queue_service()
	{
		_io_service.stop();
	}

	bool queue_service::stopped() const
	{
		return _io_service.stopped();
	}

	void queue_service::start()
	{
		if (!stopped())
			return;

		_io_service.reset();
		_work = make_unique<io_service::work>(_io_service);
	}

	void queue_service::stop()
	{
		if (stopped())
			return;

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
}