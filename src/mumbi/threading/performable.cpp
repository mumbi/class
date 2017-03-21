#include "performable.h"

namespace mumbi {
namespace threading
{
	using std::make_shared;

	performable::~performable()
	{
	}

	performable::performable(shared_ptr<invoker> invoker)		
		: invoker_holder(invoker)
	{
	}	

	void performable::perform() const
	{
		invoke();
	}
}}