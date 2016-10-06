#pragma once

#ifndef MUMBI__THREADING__SPECIFIC_THREAD_PERFORMER__H
#define MUMBI__THREADING__SPECIFIC_THREAD_PERFORMER__H

#include "performer.h"

#include <memory>

namespace mumbi {
namespace threading
{
	using std::unique_ptr;

	class specific_thread_performer final
		: public performer
	{
	public:
		~specific_thread_performer();
		specific_thread_performer();

		size_t post_count() const;

		bool stopped() const;
		void start();
		void stop();

		bool post(performable_interface_ptr performable) override;
		void run();

	private:
		class impl;
		unique_ptr<impl>	_pimpl;
	};
}}

#endif	// MUMBI__THREADING__SPECIFIC_THREAD_PERFORMER__H