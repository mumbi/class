#include "timer.h"

namespace mumbi {
namespace time
{
	bool timer::stopped() const
	{
		return _stopped;
	}

	void timer::on_elapsed()
	{
		if (_stopped)
			return;

		const clock_type::time_point now = clock_type::now();

		if (_is_first_update)
		{
			internal_update(0);

			_is_first_update = false;
		}
		else
		{
			const duration<double> elapsed = now - _last_update_time;
			const double elapsed_seconds = elapsed.count();

			if (_fixed_update_time > 0.0)
			{
				_remained_elapsed_seconds += elapsed_seconds;

				while (_remained_elapsed_seconds > _fixed_update_time)
				{
					_remained_elapsed_seconds -= _fixed_update_time;

					internal_update(_fixed_update_time);
				}
			}
			else
			{
				internal_update(elapsed_seconds);
			}
		}

		_last_update_time = now;

	}

	bool timer::internal_update(double elapsed_time)
	{
		if (!(_callable && _callable(elapsed_time, _fps)))
			_stopped = true;

		set_fps(elapsed_time);

		return true;
	}

	void timer::set_fps(double elapsed_time)
	{
		_accumulated_elapsed_time += elapsed_time;
		++_frame_count;
		if (_accumulated_elapsed_time > 1.0)
		{
			_fps = _frame_count / _accumulated_elapsed_time;

			_frame_count = 0;
			_accumulated_elapsed_time -= static_cast<int>(_accumulated_elapsed_time);
		}
	}
}}