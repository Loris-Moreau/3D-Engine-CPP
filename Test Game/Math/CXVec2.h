#pragma once

#include "..\CXPrerequisites.h"

class  CXVec2
{
public:
	CXVec2() = default;
	CXVec2(f32 x, f32 y) :x(x), y(y) {}
	CXVec2(const  CXVec2& vector) :x(vector.x), y(vector.y) {}
	
	CXVec2 operator *(f32 num) const
	{
		return {x * num, y * num};
	}
	CXVec2 operator +(const  CXVec2& vec) const
	{
		return {x + vec.x, y + vec.y};
	}
	CXVec2 operator -(const  CXVec2& vec) const
	{
		return {x - vec.x, y - vec.y};
	}
	
	f32 x = 0.0f, y = 0.0f;
};
