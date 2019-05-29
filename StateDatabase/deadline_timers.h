#pragma once
#include "pch.h"

_BEGIN_STATEDB_UTILS
/*
class deadline_timers
{
public:
	static boost::optional<boost::asio::deadline_timer>* start_timer(
		boost::asio::io_context& io, 
		const boost::posix_time::milliseconds dur, 
		boost::function<void(const BOOST_ERR_CODE&)> th
	)
	{
		auto* dt = new boost::asio::deadline_timer(io, dur);
		auto* dtOpt = new boost
		dt->async_wait(boost::bind(&deadline_timers::_timer_callback, dtOpt, boost::asio::placeholders::error, th));
		return 
	}
private:
	static void _timer_callback(boost::optional<boost::asio::deadline_timer>& timer, const BOOST_ERR_CODE& ec, boost::function<void(const BOOST_ERR_CODE&)> th)
	{
		auto* ptr = timer.get_ptr();
		timer = boost::none;
		delete ptr;
		th(ec);
	}
};*/

_END_STATEDB_UTILS