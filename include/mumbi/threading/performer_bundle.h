#pragma once

#ifndef MUMBI__THREADING__PERFORMER_BUNDLE__H
#define MUMBI__THREADING__PERFORMER_BUNDLE__H

#include "performer.h"

#include <memory>

namespace mumbi {
namespace threading
{
	using std::unique_ptr;
	using std::shared_ptr;

	class performable;	
	class dedicated_performer;

	class performer_bundle final
		: public performer
	{
	public:
		~performer_bundle();
		performer_bundle();
		
		bool stopped() const;

		void restart();
		void stop();
		
		void run();
		
		void post(performable& performable) override;

		size_t identical_performer_count() const;
		dedicated_performer& get_identical_performer(int id);

		dedicated_performer& get_running_thread_performer();		

	private:
		class impl;
		unique_ptr<impl>	_pimpl;
	};
}}

#endif	// MUMBI__THREADING__PERFORMER_BUNDLE__H
