#include "specific_thread_timer_task.h"

#include "specific_thread_performer.h"
#include "dependent_task.h"

namespace mumbi {
namespace threading {
namespace task
{
	void specific_thread_timer_task::cone()
	{
		if (_on_timed)
			_specific_thread_performer.post(make_shared_dependent_task(_on_timed, shared_from_this()));
	}
}}}
