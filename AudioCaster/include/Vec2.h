#pragma once

#include <raylib.h>
#include <math.h>

/* A wrapper class for raylib's Vector2 class*/
struct Vec2
{
	Vector2 v;
	float& x = v.x;
	float& y = v.y;

	Vec2() : v{ 0.0f, 0.0f } {}
	Vec2(float xy) : v{ xy, xy } {}
	Vec2(float x, float y) : v{ x, y } {}
	Vec2(const Vec2& v) : v{ v.x, v.y } {}
	
	void operator=(const Vec2& other)
	{
		x = other.x;
		y = other.y;
	}

	Vec2 operator+(const Vec2 &other)
	{
		return Vec2{ x + other.x, y + other.y };
	}

	Vec2 operator-(const Vec2 &other)
	{
		return Vec2{ x - other.x, y - other.y };
	}

	Vec2 operator*(const float &a)
	{
		return Vec2{ a * x, a * y };
	}

	void operator+=(const Vec2 &other)
	{
		x += other.x;
		y += other.y;
	}

	void operator-=(const Vec2 &other)
	{
		x -= other.x;
		y -= other.y;
	}

	void operator*=(const float a)
	{
		x *= a;
		y *= a;
	}

	void normalize()
	{
		float l = (float)sqrt(v.x * v.x + v.y * v.y);
		x /= l;
		y /= l;
	}
};