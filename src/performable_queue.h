#pragma once

#ifndef PERFORMABLE_QUEUE__H
#define PERFORMABLE_QUEUE__H

#include <memory>
#include <atomic>

#include "queue_service.h"
#include "event_signal.h"

namespace mumbi {
namespace threading
{
	using std::atomic;
	using mumbi::signal::event_signal;

	class performable_interface;
	using performable_interface_ptr = std::shared_ptr<performable_interface>;

	class performable_queue final
		: public queue_service
	{
	public:
		event_signal<performable_queue, void(const std::thread::id&)>	post_finished;

	public:
		performable_queue();

		size_t post_count() const;
		size_t working_count() const;
	
		void post(performable_interface_ptr performable);

	private:
		void on_posted(performable_interface_ptr performable);

	private:
		atomic<size_t>	_post_count;
		atomic<size_t>	_working_count;
	};
}}

#endif	// PERFORMABLE_QUEUE__H