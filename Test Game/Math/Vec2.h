#pragma once

#include "..\Prerequisites.h"

class  Vec2
{
public:
	Vec2() = default;
	Vec2(f32 x, f32 y) :x(x), y(y) {}
	Vec2(const  Vec2& vector) :x(vector.x), y(vector.y) {}
	
	Vec2 operator *(f32 num) const
	{
		return {x * num, y * num};
	}
	Vec2 operator +(const  Vec2& vec) const
	{
		return {x + vec.x, y + vec.y};
	}
	Vec2 operator -(const  Vec2& vec) const
	{
		return {x - vec.x, y - vec.y};
	}
	
	f32 x = 0.0f, y = 0.0f;
};
