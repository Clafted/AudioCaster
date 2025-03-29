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
	
	void operator=(const Vec2 &other)
	{
		v = Vector2{ other.x, other.y };
	}

	Vec2 operator+(Vec2 other)
	{
		return Vec2(x + other.x, y + other.y);
	}

	Vec2 operator-(Vec2 other)
	{
		return Vec2(x - other.x, y - other.y);
	}

	Vec2 operator*(float a)
	{
		return Vec2(a * x, a * y);
	}

	void operator+=(Vec2 other)
	{
		v.x += other.x;
		v.y += other.y;
	}

	void operator-=(Vec2 other)
	{
		v.x -= other.x;
		v.y -= other.y;
	}

	void operator*=(float a)
	{
		v.x *= a;
		v.y *= a;
	}

	void normalize()
	{
		float l = sqrt(pow(v.x, 2) + pow(v.y, 2));
		v = Vector2{ v.x / l, v.y / l };
	}
};