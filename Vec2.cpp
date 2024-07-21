#include "Vec2.h"
#include <math.h>

Vec2::Vec2()
{

}

Vec2::Vec2(float xin, float yin)
	: x(xin)
	, y(yin)
{
	
}

Vec2 Vec2::operator + (const Vec2& rhs) const
{
	return Vec2(x + rhs.x, y + rhs.y);
}

Vec2 Vec2::operator - (const Vec2& rhs) const
{
	return Vec2(x - rhs.x, y - rhs.y);
}

Vec2 Vec2::operator * (const float val) const
{
	return Vec2(x*val, y*val);
}

Vec2 Vec2::operator / (const float val) const
{
	return Vec2(x/val, y/val);
}

bool Vec2::operator == (const Vec2& rhs) const
{
	return (x == rhs.x && y == rhs.y);
}

bool Vec2::operator != (const Vec2& rhs) const
{
	return (x != rhs.x && y != rhs.y);
}

void Vec2::operator += (const Vec2& rhs)
{
	Vec2(x += rhs.x, y += rhs.y);
}

void Vec2::operator -= (const Vec2& rhs)
{
	Vec2(x -= rhs.x, y -= rhs.y);
}

void Vec2::operator *= (const float val)
{
	x *= val;
	y *= val;
}

void Vec2::operator /= (const float val)
{
	x /= val;
	y /= val;
}

float Vec2::dist(const Vec2& rhs) const
{
	float mpx = x - rhs.x;
	float mpy = y - rhs.y;
	return sqrt((mpx*mpx) + (mpy*mpy)) ;
}

Vec2 Vec2::normalize() const
{
	float len = sqrt((x * x) + (y * y));

	if (len > 0)
	{
		return Vec2(x/len, y/len);
	}
	else
	{
		return Vec2(0, 0);
	}
}

float Vec2::length() const {
	return sqrt(x * x + y * y);
}