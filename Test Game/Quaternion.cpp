#include "Quaternion.h"

const Quaternion Quaternion::identity(0.0f, 0.0f, 0.0f, 1.0f);

Quaternion::Quaternion(float xP, float yP, float zP, float wP)
{
	set(xP, yP, zP, wP);
}

Quaternion::Quaternion(const Vector3D& axis, float angle)
{
	float scalar = Maths::sin(angle / 2.0f);
	m_x = axis.m_x * scalar;
	m_y = axis.m_y * scalar;
	m_z = axis.m_z * scalar;
	w = Maths::cos(angle / 2.0f);
}

void Quaternion::set(float inX, float inY, float inZ, float inW)
{
	m_x = inX;
	m_y = inY;
	m_z = inZ;
	w = inW;
}

void Quaternion::conjugate()
{
	m_x *= -1.0f;
	m_y *= -1.0f;
	m_z *= -1.0f;
}

void Quaternion::normalize()
{
	float len = length();
	m_x /= len;
	m_y /= len;
	m_z /= len;
	w /= len;
}