#pragma once


class Time
{
public:
	Time() : Time(0) {}
	~Time() {}

	static Time fromMilliseconds(float milliseconds);
	static Time fromSeconds(float seconds);

	float toMilliseconds() const;
	float toSeconds() const;

	Time& operator +=(const Time& other_time);
	Time& operator -=(const Time& other_time);
	bool operator <(const Time& other_time);
	bool operator >(const Time& other_time);

	friend Time operator +(const Time& time1, const Time& time2);
	friend Time operator -(const Time& time1, const Time& time2);

protected:
	Time(float milliseconds);

protected:
	float m_TimeMS;
};

Time operator +(const Time& time1, const Time& time2);
Time operator -(const Time& time1, const Time& time2);
