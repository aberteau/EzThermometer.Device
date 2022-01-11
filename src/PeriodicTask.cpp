#include "Arduino.h"
#include "PeriodicTask.h"

PeriodicTask::PeriodicTask(unsigned long pPeriod, func func)
{
	_lastMillis = 0;
	_period = pPeriod;
	_func = func;
}

void PeriodicTask::loop()
{
	unsigned long curMillis = millis();
	if ((_lastMillis == 0) || (curMillis - _lastMillis >= _period))
	{
		_lastMillis = curMillis;
		_func();
	}
}
