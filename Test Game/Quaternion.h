#pragma once

#include "Vector3D.h"

class Quaternion
{
public:
	float m_x;
	float m_y;
	float m_z;
	float w;

	Quaternion()
	{
		*this = Quaternion::identity;
	}

	// This directly sets the quaternion components --
	// don't use for axis/angle
	explicit Quaternion(float inX, float inY, float inZ, float inW);

	// Construct the quaternion from an axis and angle
	// It is assumed that axis is already normalized,
	// and the angle is in radians
	explicit Quaternion(const Vector3D& axis, float angle);

	void set(float inX, float inY, float inZ, float inW);
	void conjugate();
	void normalize();

	float lengthSq() const
	{
		return (m_x * m_x + m_y * m_y + m_z * m_z + w * w);
	}

	float length() const
	{
		return Maths::sqrt(lengthSq());
	}

	// Normalize the provided quaternion
	static Quaternion normalize(const Quaternion& q)
	{
		Quaternion retVal = q;
		retVal.normalize();
		return retVal;
	}

	// Linear interpolation
	static Quaternion lerp(const Quaternion& a, const Quaternion& b, float f)
	{
		Quaternion retVal;
		retVal.m_x = Maths::lerp(a.m_x, b.m_x, f);
		retVal.m_y = Maths::lerp(a.m_y, b.m_y, f);
		retVal.m_z = Maths::lerp(a.m_z, b.m_z, f);
		retVal.w = Maths::lerp(a.w, b.w, f);
		retVal.normalize();
		return retVal;
	}

	static float dot(const Quaternion& a, const Quaternion& b)
	{
		return a.m_x * b.m_x + a.m_y * b.m_y + a.m_z * b.m_z + a.w * b.w;
	}

	// Spherical Linear Interpolation
	static Quaternion slerp(const Quaternion& a, const Quaternion& b, float f)
	{
		float rawCosm = Quaternion::dot(a, b);

		float cosom = -rawCosm;
		if (rawCosm >= 0.0f)
		{
			cosom = rawCosm;
		}

		float scale0, scale1;

		if (cosom < 0.9999f)
		{
			const float omega = Maths::acos(cosom);
			const float invSin = 1.f / Maths::sin(omega);
			scale0 = Maths::sin((1.f - f) * omega) * invSin;
			scale1 = Maths::sin(f * omega) * invSin;
		}
		else
		{
			// Use linear interpolation if the quaternions
			// are collinear
			scale0 = 1.0f - f;
			scale1 = f;
		}

		if (rawCosm < 0.0f)
		{
			scale1 = -scale1;
		}

		Quaternion retVal;
		retVal.m_x = scale0 * a.m_x + scale1 * b.m_x;
		retVal.m_y = scale0 * a.m_y + scale1 * b.m_y;
		retVal.m_z = scale0 * a.m_z + scale1 * b.m_z;
		retVal.w = scale0 * a.w + scale1 * b.w;
		retVal.normalize();
		return retVal;
	}

	// Concatenate
	// Rotate by q FOLLOWED BY p
	static Quaternion concatenate(const Quaternion& q, const Quaternion& p)
	{
		Quaternion retVal;

		// Vector component is:
		// ps * qv + qs * pv + pv m_x qv
		Vector3D qv(q.m_x, q.m_y, q.m_z);
		Vector3D pv(p.m_x, p.m_y, p.m_z);
		Vector3D newVec = p.w * qv + q.w * pv + Vector3D::cross(pv, qv);
		retVal.m_x = newVec.m_x;
		retVal.m_y = newVec.m_y;
		retVal.m_z = newVec.m_z;

		// Scalar component is:
		// ps * qs - pv . qv
		retVal.w = p.w * q.w - Vector3D::dot(pv, qv);

		return retVal;
	}

	static const Quaternion identity;
};

