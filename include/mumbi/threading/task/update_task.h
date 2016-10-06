#pragma once

#ifndef MUMBI__THREADING__TASK__UPDATE_TASK__H
#define MUMBI__THREADING__TASK__UPDATE_TASK__H

#include "task.h"
#include <chrono>
#include <functional>

namespace mumbi {
namespace threading {

	class performer;

namespace task
{
	using std::enable_shared_from_this;
	using std::atomic;
	using std::shared_ptr;
	using std::chrono::high_resolution_clock;
	using std::chrono::duration;

	class update_task
		: public task<void>
		, public enable_shared_from_this<update_task>
	{
		using clock_type	= high_resolution_clock;
		using duration_type	= duration<double>;
		using update_type	= function<bool(double)>;
		using poster_type	= function<void(shared_ptr<update_task>)>;
		using result_type	= function<void(void)>;

	public:
		template<typename Poster, typename F, typename Result>
		update_task(Poster&& poster, F&& f, Result&& result, bool immediately_update = true)
			: task<void>(bind(&update_task::internal_update, this))			
			, _poster(forward<Poster>(poster))
			, _update(forward<F>(f))
			, _result(forward<Result>(result))
			, _is_first_update(immediately_update)
			, _last_update_time(clock_type::now())
			, _fixed_update_time(0.0)
			, _extra_elapsed_time(0.0)
			, _fps(0)
			, _frame_count(0)
			, _accumulated_elapsed_time(0.0)
			, _stopped(false)
		{
		}

		template<typename Poster, typename F, typename Result>
		update_task(Poster&& poster, F&& f, int fps, Result&& result, bool immediately_update = true)
			: task<void>(bind(&update_task::internal_update, this))			
			, _poster(forward<Poster>(poster))
			, _update(forward<F>(f))
			, _result(forward<Result>(result))
			, _is_first_update(immediately_update)
			, _last_update_time(clock_type::now())
			, _fixed_update_time(fps > 0 ? (1.0 / fps) : 0.0)
			, _extra_elapsed_time(0.0)
			, _fps(0)
			, _frame_count(0)
			, _accumulated_elapsed_time(0.0)
			, _stopped(false)
		{
		}

		template<typename Poster, typename F, typename Rep, typename Period, typename Result>
		update_task(Poster&& poster, F&& f, const duration<Rep, Period>& d, Result&& result, bool immediately_update = true)
			: task<void>(bind(&update_task::internal_update, this))			
			, _poster(forward<Poster>(poster))
			, _update(forward<F>(f))
			, _result(forward<Result>(result))
			, _is_first_update(immediately_update)
			, _last_update_time(clock_type::now())
			, _fixed_update_time(duration_cast<duration<double>>(d).count())
			, _extra_elapsed_time(0.0)
			, _fps(0)
			, _frame_count(0)
			, _accumulated_elapsed_time(0.0)
			, _stopped(false)
		{
		}

		void stop();		

	private:
		void internal_update();	
		bool update(double elapsed_time);
		void set_fps(double elapsed_time);

	private:		
		poster_type				_poster;
		update_type				_update;
		result_type				_result;

		bool					_is_first_update;
		clock_type::time_point	_last_update_time;

		// for fixed update.
		const double			_fixed_update_time;
		double					_extra_elapsed_time;

		// for fps.
		int						_fps;
		int						_frame_count;
		double					_accumulated_elapsed_time;

		atomic<bool>			_stopped;
	};
}}}

#endif	// MUMBI__THREADING__TASK__UPDATE_TASK__H
