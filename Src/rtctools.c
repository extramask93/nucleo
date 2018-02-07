/*
 * rtctools.c
 *
 *  Created on: 04.02.2018
 *      Author: LENOVO
 */

#include "rtctools.h"

uint32_t DifferenceBetweenTimePeriod(RTC_TimeTypeDef *start, RTC_TimeTypeDef *stop)
{
	uint32_t tim1 = start->Seconds + start->Minutes*60 + start->Hours*3600;
	uint32_t tim2 = stop->Seconds + stop->Minutes*60 + stop->Hours*3600;
	return tim2-tim1;
}
void AddTime(RTC_TimeTypeDef *time2, uint8_t seconds)
{
	time2->Seconds += seconds%60; seconds /=60;
	time2->Minutes += seconds%60; seconds /=60;
	time2->Hours += seconds%24; seconds/=24;

	if(time2->Seconds > 60)
	{
		time2->Minutes += 1;
		time2->Seconds -= 60;
	}

	if(time2->Minutes > 60)
	{
		time2->Hours += 1;
		time2->Minutes -= 60;
	}

	if(time2->Hours > 24)
	{
		time2->Hours -= 24;
	}

}
