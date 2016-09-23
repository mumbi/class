#include "performable_queue.h"

#include <functional>
#include "performable_interface.h"

namespace mumbi {
namespace threading
{
	namespace this_thread = std::this_thread;

	performable_queue::performable_queue()
		: _post_count(0)
		, _working_count(0)
	{
	}

	size_t performable_queue::post_count() const
	{
		return _post_count;
	}

	size_t performable_queue::working_count() const
	{
		return _working_count;
	}

	void performable_queue::post(performable_interface_ptr performable)
	{
		get_io_service().post(bind(&performable_queue::on_posted, this, performable));

		++_post_count;
	}	

	void performable_queue::on_posted(performable_interface_ptr performable)
	{	
		const auto thread_id = this_thread::get_id();

		++_working_count;
		
		performable->perform();

		--_working_count;
		--_post_count;		

		post_finished.emit(thread_id);
	}
}}
