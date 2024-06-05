#pragma once
#include "..\CXPrerequisites.h"

class  CXVec3
{
public:
	CXVec3() = default;
	CXVec3(f32 x, f32 y, f32 z) :x(x), y(y), z(z) {}
	CXVec3(const  CXVec3& vector) :x(vector.x), y(vector.y), z(vector.z) {}

	static CXVec3 lerp(const  CXVec3& start, const  CXVec3& end, f32 delta)
	{
		CXVec3 v;
		v.x = start.x * (1.0f - delta) + end.x * (delta);
		v.y = start.y * (1.0f - delta) + end.y * (delta);
		v.z = start.z * (1.0f - delta) + end.z * (delta);
		return v;
	}

	CXVec3 operator *(f32 num) const
	{
		return {x * num, y * num, z * num};
	}

	CXVec3 operator +(const  CXVec3& vec) const
	{
		return {x + vec.x, y + vec.y, z + vec.z};
	}

	CXVec3 operator -(const  CXVec3& vec) const
	{
		return {x - vec.x, y - vec.y, z - vec.z};
	}

	f32 length() const
	{
		return (f32)sqrt((x * x) + (y * y) + (z * z));
	}

	static CXVec3 normalize(const  CXVec3& vec)
	{
		CXVec3 res;
		f32 len = (f32)sqrt((f32)(vec.x * vec.x) + (f32)(vec.y * vec.y) + (f32)(vec.z * vec.z));
		if (!len)
		{
			return {};
		}

		res.x = vec.x / len;
		res.y = vec.y / len;
		res.z = vec.z / len;

		return res;
	}

	static CXVec3 cross(const  CXVec3& v1, const  CXVec3& v2)
	{
		CXVec3 res;
		res.x = (v1.y * v2.z) - (v1.z * v2.y);
		res.y = (v1.z * v2.x) - (v1.x * v2.z);
		res.z = (v1.x * v2.y) - (v1.y * v2.x);
		return res;
	}
	
	f32 x = 0.0f, y = 0.0f, z = 0.0f;
};
