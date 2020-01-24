#include "Engine/Pch.h"

#include "Engine/GameStates/Time.h"

Time Time::fromMilliseconds(float milliseconds)
{
	return Time(milliseconds);
}

Time Time::fromSeconds(float seconds)
{
	return Time(seconds * 1000);
}

float Time::toMilliseconds() const
{
	return m_TimeMS;
}

float Time::toSeconds() const
{
	return m_TimeMS / 1000;
}

Time& Time::operator+=(const Time& other_time)
{
	m_TimeMS += other_time.toMilliseconds();
	return *this;
}

Time& Time::operator -=(const Time& other_time)
{
	m_TimeMS -= other_time.toMilliseconds();
	return *this;
}

bool Time::operator <(const Time& other_time)
{
	return m_TimeMS < other_time.m_TimeMS;
}

bool Time::operator >(const Time& other_time)
{
	return m_TimeMS > other_time.m_TimeMS;
}

Time operator +(const Time& time1, const Time& time2)
{
	return Time::fromMilliseconds(time1.m_TimeMS + time2.m_TimeMS);
}

Time operator -(const Time& time1, const Time& time2)
{
	return Time::fromMilliseconds(time1.m_TimeMS - time2.m_TimeMS);
}

Time::Time(float milliseconds)
	: m_TimeMS(milliseconds)
{
}