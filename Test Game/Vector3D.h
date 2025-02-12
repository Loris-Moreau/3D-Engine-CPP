#pragma once

#include "Maths.h"

class Vector3D
{

public:
	float m_x = 0.0f, m_y = 0.0f, m_z = 0.0f;

	Vector3D() {}
	
	Vector3D(float xP, float yP, float zP) : m_x(xP), m_y(yP), m_z(zP) {}
	Vector3D(const Vector3D& vector) : m_x(vector.m_x), m_y(vector.m_y), m_z(vector.m_z) {}
	
	~Vector3D() {}
	
	void set(float xP, float yP, float zP);
	float lengthSq() const;
	float length() const;
	void normalize();

	const float* getAsFloatPtr() const
	{
		return reinterpret_cast<const float*>(&m_x);
	}

	// Vector addition (a + b)
	friend Vector3D operator+(const Vector3D& a, const Vector3D& b)
	{
		return Vector3D(a.m_x + b.m_x, a.m_y + b.m_y, a.m_z + b.m_z);
	}
	
	// Vector subtraction (a - b)
	friend Vector3D operator-(const Vector3D& a, const Vector3D& b)
	{
		return Vector3D(a.m_x - b.m_x, a.m_y - b.m_y, a.m_z - b.m_z);
	}

	// Component-wise multiplication
	friend Vector3D operator*(const Vector3D& left, const Vector3D& right)
	{
		return Vector3D(left.m_x * right.m_x, left.m_y * right.m_y, left.m_z * right.m_z);
	}

	// Scalar multiplication
	friend Vector3D operator*(const Vector3D& vec, float scalar)
	{
		return Vector3D(vec.m_x * scalar, vec.m_y * scalar, vec.m_z * scalar);
	}

	// Scalar multiplication
	friend Vector3D operator*(float scalar, const Vector3D& vec)
	{
		return Vector3D(vec.m_x * scalar, vec.m_y * scalar, vec.m_z * scalar);
	}

	// Scalar *=
	Vector3D& operator*=(float scalar)
	{
		m_x *= scalar;
		m_y *= scalar;
		m_z *= scalar;
		return *this;
	}

	// Vector +=
	Vector3D& operator+=(const Vector3D& right)
	{
		m_x += right.m_x;
		m_y += right.m_y;
		m_z += right.m_z;
		return *this;
	}

	// Vector -=
	Vector3D& operator-=(const Vector3D& right)
	{
		m_x -= right.m_x;
		m_y -= right.m_y;
		m_z -= right.m_z;
		return *this;
	}

	// Normalize the provided vector
	/*
	static Vector3D normalize(const Vector3D& vec)
	{
		Vector3D temp = vec;
		temp.normalize();
		return temp;
	}
	*/
	static Vector3D normalize(const Vector3D& vec)
	{
		Vector3D res;
		float len = (float)sqrt((float)(vec.m_x * vec.m_x) + (float)(vec.m_y * vec.m_y) + (float)(vec.m_z * vec.m_z));
		if (!len)
		{
			return Vector3D();
		}
		
		res.m_x = vec.m_x / len;
		res.m_y = vec.m_y / len;
		res.m_z = vec.m_z / len;
		
		return res;
	}
	
	// Dot product between two vectors (a dot b)
	static float dot(const Vector3D& a, const Vector3D& b)
	{
		return (a.m_x * b.m_x + a.m_y * b.m_y + a.m_z * b.m_z);
	}

	// Cross product between two vectors (a cross b)
	static Vector3D cross(const  Vector3D& v1, const  Vector3D& v2)
	{
		Vector3D res;
		res.m_x = (v1.m_y * v2.m_z) - (v1.m_z * v2.m_y);
		res.m_y = (v1.m_z * v2.m_x) - (v1.m_x * v2.m_z);
		res.m_z = (v1.m_x * v2.m_y) - (v1.m_y * v2.m_x);
		return res;
	}
	
	// Lerp from A to B by f
	/*
	static Vector3D lerp(const Vector3D& a, const Vector3D& b, float f)
	{
		return Vector3D(a + f * (b - a));
	}
	*/
	static Vector3D lerp(const Vector3D& start, const Vector3D& end, float delta)
	{
		Vector3D v;
		v.m_x = start.m_x * (1.0f - delta) + end.m_x * (delta);
		v.m_y = start.m_y * (1.0f - delta) + end.m_y * (delta);
		v.m_z = start.m_z * (1.0f - delta) + end.m_z * (delta);
		return v;
	}

	// Reflect V about (normalized) N
	static Vector3D reflect(const Vector3D& v, const Vector3D& n)
	{
		return v - 2.0f * Vector3D::dot(v, n) * n;
	}

	static Vector3D transform(const Vector3D& vec, const class Matrix4x4& mat, float w = 1.0f);

	// This will transform the vector and renormalize the w component
	static Vector3D transformWithPerspDiv(const Vector3D& vec, const class Matrix4x4& mat, float w = 1.0f);

	// Transform a Vector3D by a quaternion
	static Vector3D transform(const Vector3D& v, const class Quaternion& q);

	static const Vector3D zero;
	static const Vector3D unitX;
	static const Vector3D unitY;
	static const Vector3D unitZ;
	static const Vector3D negUnitX;
	static const Vector3D negUnitY;
	static const Vector3D negUnitZ;
	static const Vector3D infinity;
	static const Vector3D negInfinity;
};

