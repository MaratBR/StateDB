#pragma once
#include "pch.h"

_BEGIN_STATEDB_UTILS

template<typename TId, typename ... TArgs>
class dispatcher
{
public:
	using function_type = boost::function<void(TArgs...)>;

	// Registers handler for given id.
	void register_handler(TId id, function_type fn)
	{
		handlers[id] = fn;
	}

	// Registers functor handler for given id.
	template<typename TFunctorHandler>
	void register_handler(TId id)
	{
		handlers[id] = TFunctorHandler();
	}

	void clear(TId id)
	{
		handlers.erase(id);
	}

	bool call(TId id, TArgs ... args)
	{
		auto it = handlers.find(id);
		if (it == handlers.end())
			return false;
		it->second(args...);
		return true;
	}
private:
	std::map<TId, function_type> handlers;
};

_END_STATEDB_UTILS