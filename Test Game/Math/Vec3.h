#pragma once
#include "..\Prerequisites.h"

class  Vec3
{
public:
	Vec3() = default;
	Vec3(f32 x, f32 y, f32 z) :x(x), y(y), z(z) {}
	Vec3(const  Vec3& vector) :x(vector.x), y(vector.y), z(vector.z) {}

	static Vec3 lerp(const  Vec3& start, const  Vec3& end, f32 delta)
	{
		Vec3 v;
		v.x = start.x * (1.0f - delta) + end.x * (delta);
		v.y = start.y * (1.0f - delta) + end.y * (delta);
		v.z = start.z * (1.0f - delta) + end.z * (delta);
		return v;
	}

	Vec3 operator *(f32 num) const
	{
		return {x * num, y * num, z * num};
	}

	Vec3 operator +(const  Vec3& vec) const
	{
		return {x + vec.x, y + vec.y, z + vec.z};
	}

	Vec3 operator -(const  Vec3& vec) const
	{
		return {x - vec.x, y - vec.y, z - vec.z};
	}

	f32 length() const
	{
		return (f32)sqrt((x * x) + (y * y) + (z * z));
	}

	static Vec3 normalize(const  Vec3& vec)
	{
		Vec3 res;
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

	static Vec3 cross(const  Vec3& v1, const  Vec3& v2)
	{
		Vec3 res;
		res.x = (v1.y * v2.z) - (v1.z * v2.y);
		res.y = (v1.z * v2.x) - (v1.x * v2.z);
		res.z = (v1.x * v2.y) - (v1.y * v2.x);
		return res;
	}
	
	f32 x = 0.0f, y = 0.0f, z = 0.0f;
};
