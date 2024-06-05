#pragma once

#include "CXVec3.h"

class  CXVec4
{
public:
	CXVec4() = default;
	CXVec4(f32 x, f32 y, f32 z, f32 w) :x(x), y(y), z(z), w(w) {}
	CXVec4(const  CXVec4& vector) :x(vector.x), y(vector.y), z(vector.z), w(vector.w) {}
	CXVec4(const  CXVec3& vector) :x(vector.x), y(vector.y), z(vector.z), w(1.0) {}

	void cross(CXVec4& v1, CXVec4& v2, CXVec4& v3)
	{
		this->x = v1.y * (v2.z * v3.w - v3.z * v2.w) - v1.z * (v2.y * v3.w - v3.y * v2.w) + v1.w * (v2.y * v3.z - v2.z * v3.y);
		this->y = -(v1.x * (v2.z * v3.w - v3.z * v2.w) - v1.z * (v2.x * v3.w - v3.x * v2.w) + v1.w * (v2.x * v3.z - v3.x * v2.z));
		this->z = v1.x * (v2.y * v3.w - v3.y * v2.w) - v1.y * (v2.x * v3.w - v3.x * v2.w) + v1.w * (v2.x * v3.y - v3.x * v2.y);
		this->w = -(v1.x * (v2.y * v3.z - v3.y * v2.z) - v1.y * (v2.x * v3.z - v3.x * v2.z) + v1.z * (v2.x * v3.y - v3.x * v2.y));
	}
	
	f32 x = 0.0f, y = 0.0f, z = 0.0f, w = 0.0f;
};
