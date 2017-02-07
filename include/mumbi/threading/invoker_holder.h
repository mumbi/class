#pragma once

#ifndef MUMBI__THREADING__INVOKER_HOLDER__H
#define MUMBI__THREADING__INVOKER_HOLDER__H

#include <memory>

namespace mumbi {
namespace threading
{
	using std::shared_ptr;
	using std::static_pointer_cast;

	class invoker;

	class invoker_holder
	{
	public:
		explicit invoker_holder(shared_ptr<invoker> invoker);

		bool invoke();

	protected:
		shared_ptr<const invoker> get_invoker() const;
		shared_ptr<invoker> get_invoker();

		template<class Invoker>
		shared_ptr<Invoker> get_invoker()
		{
			return static_pointer_cast<Invoker>(_invoker);
		}

		template<class Invoker>
		shared_ptr<Invoker> get_invoker() const
		{
			return const_cast<invoker_holder*>(this)->get_invoker<Invoker>();
		}

	private:
		shared_ptr<invoker>	_invoker;
	};
}}

#endif	// MUMBI__THREADING__INVOKER_HOLDER__H