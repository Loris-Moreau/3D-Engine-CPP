#pragma once

#include "Vec3.h"

class  Vec4
{
public:
	Vec4() = default;
	Vec4(f32 x, f32 y, f32 z, f32 w) :x(x), y(y), z(z), w(w) {}
	Vec4(const  Vec4& vector) :x(vector.x), y(vector.y), z(vector.z), w(vector.w) {}
	Vec4(const  Vec3& vector) :x(vector.x), y(vector.y), z(vector.z), w(1.0) {}

	void cross(Vec4& v1, Vec4& v2, Vec4& v3)
	{
		this->x = v1.y * (v2.z * v3.w - v3.z * v2.w) - v1.z * (v2.y * v3.w - v3.y * v2.w) + v1.w * (v2.y * v3.z - v2.z * v3.y);
		this->y = -(v1.x * (v2.z * v3.w - v3.z * v2.w) - v1.z * (v2.x * v3.w - v3.x * v2.w) + v1.w * (v2.x * v3.z - v3.x * v2.z));
		this->z = v1.x * (v2.y * v3.w - v3.y * v2.w) - v1.y * (v2.x * v3.w - v3.x * v2.w) + v1.w * (v2.x * v3.y - v3.x * v2.y);
		this->w = -(v1.x * (v2.y * v3.z - v3.y * v2.z) - v1.y * (v2.x * v3.z - v3.x * v2.z) + v1.z * (v2.x * v3.y - v3.x * v2.y));
	}
	
	f32 x = 0.0f, y = 0.0f, z = 0.0f, w = 0.0f;
};
