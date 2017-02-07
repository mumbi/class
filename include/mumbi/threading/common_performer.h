#pragma once

#ifndef MUMBI__THREADING__COMMON_PERFORMER__H
#define MUMBI__THREADING__COMMON_PERFORMER__H

#include "performer.h"

#include <memory>

namespace mumbi {
namespace threading
{
	using std::unique_ptr;
	using std::shared_ptr;

	class performable;	

	class common_performer final
		: public performer
	{
	public:
		~common_performer();
		common_performer();

		size_t post_count() const;
		size_t working_count() const;
		size_t thread_count() const;

		bool stopped() const;

		void restart();
		void stop();

		void start_running();
		void end_running();
		
		void post(performable& performable) override;

	private:
		class impl;
		unique_ptr<impl>	_pimpl;
	};
}}

#endif	// MUMBI__THREADING__COMMON_PERFORMER__H