#include <boost/date_time/posix_time/posix_time.hpp>
#include "HighRezTimer.h"
#include "Exception.h"


//======================================================================================================================
// Constructor                                                                                                         =
//======================================================================================================================
HighRezTimer::HighRezTimer():
	startTime(0),
	stopTime(0)
{}


//======================================================================================================================
// start                                                                                                               =
//======================================================================================================================
void HighRezTimer::start()
{
	RASSERT_THROW_EXCEPTION(startTime != 0);
	RASSERT_THROW_EXCEPTION(stopTime != 0);
	startTime = getCrntTime();
	stopTime = 0;
}


//======================================================================================================================
// stop                                                                                                                =
//======================================================================================================================
void HighRezTimer::stop()
{
	RASSERT_THROW_EXCEPTION(startTime == 0);
	RASSERT_THROW_EXCEPTION(stopTime != 0);
	stopTime = getCrntTime();
}


//======================================================================================================================
// getElapsedTime                                                                                                      =
//======================================================================================================================
uint HighRezTimer::getElapsedTime() const
{
	if(stopTime == 0)
	{
		return getCrntTime() - startTime;
	}
	else
	{
		return stopTime - startTime;
	}
}


//======================================================================================================================
// getCrntTime                                                                                                        =
//======================================================================================================================
uint HighRezTimer::getCrntTime()
{
	using namespace boost::posix_time;
	return ptime(microsec_clock::local_time()).time_of_day().total_milliseconds();
}
