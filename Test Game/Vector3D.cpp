#include "Vector3D.h"

#include "Matrix4x4.h"
#include "Quaternion.h"

const Vector3D Vector3D::zero(0.0f, 0.0f, 0.f);
const Vector3D Vector3D::unitX(1.0f, 0.0f, 0.0f);
const Vector3D Vector3D::unitY(0.0f, 1.0f, 0.0f);
const Vector3D Vector3D::unitZ(0.0f, 0.0f, 1.0f);
const Vector3D Vector3D::negUnitX(-1.0f, 0.0f, 0.0f);
const Vector3D Vector3D::negUnitY(0.0f, -1.0f, 0.0f);
const Vector3D Vector3D::negUnitZ(0.0f, 0.0f, -1.0f);
const Vector3D Vector3D::infinity(Maths::infinity, Maths::infinity, Maths::infinity);
const Vector3D Vector3D::negInfinity(Maths::negInfinity, Maths::negInfinity, Maths::negInfinity);

void Vector3D::set(float xP, float yP, float zP)
{
	m_x = xP;
	m_y = yP;
	m_z = zP;
}

float Vector3D::lengthSq() const
{
	return (m_x * m_x + m_y * m_y + m_z * m_z);
}

float Vector3D::length() const
{
	return (Maths::sqrt(lengthSq()));
}

void Vector3D::normalize()
{
	float len = length();
	m_x /= len;
	m_y /= len;
	m_z /= len;
}

Vector3D Vector3D::transform(const Vector3D& vec, const Matrix4x4& mat, float w)
{
	Vector3D retVal;
	retVal.m_x = vec.m_x * mat.mat[0][0] + vec.m_y * mat.mat[1][0] +
		vec.m_z * mat.mat[2][0] + w * mat.mat[3][0];
	retVal.m_y = vec.m_x * mat.mat[0][1] + vec.m_y * mat.mat[1][1] +
		vec.m_z * mat.mat[2][1] + w * mat.mat[3][1];
	retVal.m_z = vec.m_x * mat.mat[0][2] + vec.m_y * mat.mat[1][2] +
		vec.m_z * mat.mat[2][2] + w * mat.mat[3][2];
	//ignore w since we aren't returning a new value for it...
	return retVal;
}

Vector3D Vector3D::transformWithPerspDiv(const Vector3D& vec, const Matrix4x4& mat, float w)
{
	Vector3D retVal;
	retVal.m_x = vec.m_x * mat.mat[0][0] + vec.m_y * mat.mat[1][0] +
		vec.m_z * mat.mat[2][0] + w * mat.mat[3][0];
	retVal.m_y = vec.m_x * mat.mat[0][1] + vec.m_y * mat.mat[1][1] +
		vec.m_z * mat.mat[2][1] + w * mat.mat[3][1];
	retVal.m_z = vec.m_x * mat.mat[0][2] + vec.m_y * mat.mat[1][2] +
		vec.m_z * mat.mat[2][2] + w * mat.mat[3][2];
	float transformedW = vec.m_x * mat.mat[0][3] + vec.m_y * mat.mat[1][3] +
		vec.m_z * mat.mat[2][3] + w * mat.mat[3][3];
	if (!Maths::nearZero(Maths::abs(transformedW)))
	{
		transformedW = 1.0f / transformedW;
		retVal *= transformedW;
	}
	return retVal;
}

Vector3D Vector3D::transform(const Vector3D& v, const Quaternion& q)
{
	// v + 2.0*cross(q.xyz, cross(q.xyz,v) + q.w*v);
	Vector3D qv(q.m_x, q.m_y, q.m_z);
	Vector3D retVal = v;
	retVal += 2.0f * Vector3D::cross(qv, Vector3D::cross(qv, v) + q.w * v);
	return retVal;
}
