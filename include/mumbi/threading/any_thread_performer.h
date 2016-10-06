#pragma once

#ifndef MUMBI__THREADING__ANY_THREAD_PERFORMER__H
#define MUMBI__THREADING__ANY_THREAD_PERFORMER__H

#include "performer.h"

#include <memory>

namespace mumbi {
namespace threading
{
	using std::unique_ptr;
	using std::shared_ptr;

	class performable_interface;
	using performable_interface_ptr = shared_ptr<performable_interface>;

	class any_thread_performer final
		: public performer
	{
	public:
		~any_thread_performer();
		any_thread_performer();

		size_t post_count() const;
		size_t working_count() const;
		size_t thread_count() const;

		bool stopped() const;
		void start();
		void stop();

		bool post(performable_interface_ptr performable) override;

	private:
		class impl;
		unique_ptr<impl>	_pimpl;
	};
}}

#endif	// MUMBI__THREADING__ANY_THREAD_PERFORMER__H