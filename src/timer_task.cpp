#include "timer_task.h"

#include "any_thread_performer.h"

namespace mumbi {
namespace threading {
namespace task
{
	namespace this_thread = std::this_thread;

	using std::chrono::seconds;

	void timer_task::wait()
	{
		auto duration = _until - clock_type::now();

		if (duration < seconds(2))
		{
			this_thread::sleep_for(duration);			

			if (_on_timed)
				_on_timed();

			get_promise().set_value();
		}
		else
		{	
			this_thread::sleep_for(seconds(1));

			_performer.post(shared_from_this());
		}
	}
}}}