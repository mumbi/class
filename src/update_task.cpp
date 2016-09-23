#include "update_task.h"

#include "performer.h"

namespace mumbi {
namespace threading {
namespace task
{
	void update_task::stop()
	{
		_stopped = true;
	}

	void update_task::internal_update()
	{
		if (_stopped)
		{
			get_promise().set_value();
			return;
		}

		const clock_type::time_point now = clock_type::now();		

		if (_is_first_update)
		{			
			if (false == update(0))
				return;

			_is_first_update = false;
		}
		else
		{
			const duration<double> elapsed_duration = now - _last_update_time;
			const double elapsed_time = elapsed_duration.count();

			_extra_elapsed_time += elapsed_time;
			if (_fixed_update_time > 0.0)
			{
				while (_extra_elapsed_time > _fixed_update_time)
				{
					_extra_elapsed_time -= _fixed_update_time;

					if (false == update(_fixed_update_time))
						return;
				}
			}
			else
			{
				if (false == update(elapsed_time))
					return;
			}
		}

		_last_update_time = now;				
		
		if (_poster)
		{
			_poster(shared_from_this());
		}
	}

	bool update_task::update(double elapsed_time)
	{
		if (false == (_update && _update(elapsed_time)))
		{
			if (_result)
				_result();

			return false;
		}

		set_fps(elapsed_time);

		return true;
	}

	void update_task::set_fps(double elapsed_time)
	{
		_accumulated_elapsed_time += elapsed_time;
		++_frame_count;
		if (_accumulated_elapsed_time > 1.0)
		{
			_fps = _frame_count;

			_frame_count = 0;
			_accumulated_elapsed_time -= 1.0;
		}
	}
}}}