#pragma once

#ifndef MUMBI__THREADING__PERFORMABLE_QUEUE__H
#define MUMBI__THREADING__PERFORMABLE_QUEUE__H

#include <memory>
#include <atomic>

#include "queue_service.h"
#include "event_signal.h"

namespace mumbi {
namespace threading
{
	using std::atomic;
	using mumbi::functional::event_signal;

	class performable;	

	class performable_queue final
		: public queue_service
	{
	public:
		event_signal<performable_queue, void(const std::thread::id&)>	post_finished;

	public:
		performable_queue();

		size_t post_count() const;
		size_t working_count() const;	
		
		void post(const performable& performable);

	private:		
		void on_dispatched(const performable& performable);

	private:
		atomic<size_t>	_post_count;
		atomic<size_t>	_working_count;
	};
}}

#endif	// MUMBI__THREADING__PERFORMABLE_QUEUE__H