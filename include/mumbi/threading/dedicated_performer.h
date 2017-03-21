#pragma once

#ifndef MUMBI__THREADING__DEDICATED_PERFORMER__H
#define MUMBI__THREADING__DEDICATED_PERFORMER__H

#include "performer.h"

#include <memory>

namespace mumbi {
namespace threading
{
	using std::unique_ptr;

	class performable;

	class dedicated_performer final
		: public performer
	{
	public:
		~dedicated_performer();
		dedicated_performer();

		size_t post_count() const;
		bool stopped() const;

		void restart();
		void stop();

		void run();
		
		void post(const performable& performable) override;

	private:
		class impl;
		unique_ptr<impl>	_pimpl;
	};
}}

#endif	// MUMBI__THREADING__DEDICATED_PERFORMER__H