#pragma once

#ifndef MUMBI__TIME__UPDATER__H
#define MUMBI__TIME__UPDATER__H

#include <chrono>
#include <functional>
#include <atomic>

namespace mumbi {
namespace time
{
	using std::chrono::high_resolution_clock;
	using std::chrono::duration;
	using std::chrono::time_point;
	using std::function;
	using std::atomic;
	using std::forward;

	class updater
	{
	private:
		using clock_type = high_resolution_clock;
		using duration_type = duration<double>;
		using callable_type = function<bool(double, double)>;		// elapsed time, frame per second	

	private:
		template<typename Callable>
		updater(Callable&& callable, double fixed_update_time, bool immediately_update)
			: _callable(forward<Callable>(callable))
			, _is_first_update(immediately_update)
			, _last_update_time(clock_type::now())
			, _fixed_update_time(fixed_update_time)
			, _remained_elapsed_seconds(0.0)
			, _fps(0)
			, _frame_count(0)
			, _accumulated_elapsed_time(0.0)
			, _stopped(false)
		{
		}

	public:
		template<typename Callable>
		updater(Callable&& callable, bool immediately_update = false)
			: updater(forward<Callable>(callable), 0.0, immediately_update)
			/*: _callable(forward<Callable>(callable))
			, _is_first_update(immediately_update)
			, _last_update_time(clock_type::now())
			, _fixed_update_time(0.0)
			, _remained_elapsed_seconds(0.0)
			, _fps(0)
			, _frame_count(0)
			, _accumulated_elapsed_time(0.0)
			, _stopped(false)*/
		{
		}

		template<typename Callable>
		updater(Callable&& callable, int fps, bool immediately_update = false)
			: updater(forward<Callable>(callable), fps > 0 ? (1.0 / fps) : 0.0, immediately_update)

			/*: _callable(forward<Callable>(callable))
			, _is_first_update(immediately_update)
			, _last_update_time(clock_type::now())
			, _fixed_update_time(fps > 0 ? (1.0 / fps) : 0.0)
			, _remained_elapsed_seconds(0.0)
			, _fps(0)
			, _frame_count(0)
			, _accumulated_elapsed_time(0.0)
			, _stopped(false)*/
		{
		}

		template<typename Callable, typename Rep, typename Period>
		updater(Callable&& callable, const duration<Rep, Period>& d, bool immediately_update = false)
			: updater(forward<Callable>(callable), duration_cast<duration<double>>(d).count(), immediately_update)
			/*: _callable(forward<Callable>(callable))
			, _is_first_update(immediately_update)
			, _last_update_time(clock_type::now())
			, _fixed_update_time(duration_cast<duration<double>>(d).count())
			, _remained_elapsed_seconds(0.0)
			, _fps(0)
			, _frame_count(0)
			, _accumulated_elapsed_time(0.0)
			, _stopped(false)*/
		{
		}		

		bool stopped() const;
		void update();

	private:
		bool internal_update(double elapsed_time);
		void set_fps(double elapsed_time);

	private:
		callable_type			_callable;

		bool					_is_first_update;
		clock_type::time_point	_last_update_time;

		// for fixed update.
		const double			_fixed_update_time;
		double					_remained_elapsed_seconds;

		// for fps.
		double					_fps;
		int						_frame_count;
		double					_accumulated_elapsed_time;

		atomic<bool>			_stopped;
	};
}}

#endif	//	MUMBI__TIME__UPDATER__H