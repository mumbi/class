#include "invoker_holder.h"

#include "invoker.h"

namespace mumbi {
namespace threading
{
	invoker_holder::invoker_holder(shared_ptr<invoker> invoker)
		: _invoker(invoker)
	{
	}

	bool invoker_holder::invoke() const
	{
		if (_invoker)
			return _invoker->invoke();

		return true;
	}

	shared_ptr<const invoker> invoker_holder::get_invoker() const
	{
		return const_cast<invoker_holder*>(this)->get_invoker();;
	}

	shared_ptr<invoker> invoker_holder::get_invoker()
	{
		return _invoker;
	}	
}}