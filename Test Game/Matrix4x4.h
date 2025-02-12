#pragma once

#include <memory.h>

#include "Quaternion.h"
#include "Vector3D.h"
#include "Vector4D.h"

class Matrix4x4
{
public:
	float mat[4][4] = {};

	Matrix4x4()
	{
		*this = Matrix4x4::identity;
	}

	explicit Matrix4x4(float inMat[4][4])
	{
		memcpy(mat, inMat, 16 * sizeof(float));
	}
	
	void setIdentity()
	{
		::memset(mat, 0, sizeof(mat));
		mat[0][0] = 1;
		mat[1][1] = 1;
		mat[2][2] = 1;
		mat[3][3] = 1;
	}
	
	// Cast to a const float pointer
	const float* getAsFloatPtr() const
	{
		return reinterpret_cast<const float*>(&mat[0][0]);
	}

	// Matrix multiplication (a * b)
	friend Matrix4x4 operator*(const Matrix4x4& a, const Matrix4x4& b)
	{
		Matrix4x4 retVal;
		// row 0
		retVal.mat[0][0] =
			a.mat[0][0] * b.mat[0][0] +
			a.mat[0][1] * b.mat[1][0] +
			a.mat[0][2] * b.mat[2][0] +
			a.mat[0][3] * b.mat[3][0];

		retVal.mat[0][1] =
			a.mat[0][0] * b.mat[0][1] +
			a.mat[0][1] * b.mat[1][1] +
			a.mat[0][2] * b.mat[2][1] +
			a.mat[0][3] * b.mat[3][1];

		retVal.mat[0][2] =
			a.mat[0][0] * b.mat[0][2] +
			a.mat[0][1] * b.mat[1][2] +
			a.mat[0][2] * b.mat[2][2] +
			a.mat[0][3] * b.mat[3][2];

		retVal.mat[0][3] =
			a.mat[0][0] * b.mat[0][3] +
			a.mat[0][1] * b.mat[1][3] +
			a.mat[0][2] * b.mat[2][3] +
			a.mat[0][3] * b.mat[3][3];

		// row 1
		retVal.mat[1][0] =
			a.mat[1][0] * b.mat[0][0] +
			a.mat[1][1] * b.mat[1][0] +
			a.mat[1][2] * b.mat[2][0] +
			a.mat[1][3] * b.mat[3][0];

		retVal.mat[1][1] =
			a.mat[1][0] * b.mat[0][1] +
			a.mat[1][1] * b.mat[1][1] +
			a.mat[1][2] * b.mat[2][1] +
			a.mat[1][3] * b.mat[3][1];

		retVal.mat[1][2] =
			a.mat[1][0] * b.mat[0][2] +
			a.mat[1][1] * b.mat[1][2] +
			a.mat[1][2] * b.mat[2][2] +
			a.mat[1][3] * b.mat[3][2];

		retVal.mat[1][3] =
			a.mat[1][0] * b.mat[0][3] +
			a.mat[1][1] * b.mat[1][3] +
			a.mat[1][2] * b.mat[2][3] +
			a.mat[1][3] * b.mat[3][3];

		// row 2
		retVal.mat[2][0] =
			a.mat[2][0] * b.mat[0][0] +
			a.mat[2][1] * b.mat[1][0] +
			a.mat[2][2] * b.mat[2][0] +
			a.mat[2][3] * b.mat[3][0];

		retVal.mat[2][1] =
			a.mat[2][0] * b.mat[0][1] +
			a.mat[2][1] * b.mat[1][1] +
			a.mat[2][2] * b.mat[2][1] +
			a.mat[2][3] * b.mat[3][1];

		retVal.mat[2][2] =
			a.mat[2][0] * b.mat[0][2] +
			a.mat[2][1] * b.mat[1][2] +
			a.mat[2][2] * b.mat[2][2] +
			a.mat[2][3] * b.mat[3][2];

		retVal.mat[2][3] =
			a.mat[2][0] * b.mat[0][3] +
			a.mat[2][1] * b.mat[1][3] +
			a.mat[2][2] * b.mat[2][3] +
			a.mat[2][3] * b.mat[3][3];

		// row 3
		retVal.mat[3][0] =
			a.mat[3][0] * b.mat[0][0] +
			a.mat[3][1] * b.mat[1][0] +
			a.mat[3][2] * b.mat[2][0] +
			a.mat[3][3] * b.mat[3][0];

		retVal.mat[3][1] =
			a.mat[3][0] * b.mat[0][1] +
			a.mat[3][1] * b.mat[1][1] +
			a.mat[3][2] * b.mat[2][1] +
			a.mat[3][3] * b.mat[3][1];

		retVal.mat[3][2] =
			a.mat[3][0] * b.mat[0][2] +
			a.mat[3][1] * b.mat[1][2] +
			a.mat[3][2] * b.mat[2][2] +
			a.mat[3][3] * b.mat[3][2];

		retVal.mat[3][3] =
			a.mat[3][0] * b.mat[0][3] +
			a.mat[3][1] * b.mat[1][3] +
			a.mat[3][2] * b.mat[2][3] +
			a.mat[3][3] * b.mat[3][3];

		return retVal;
	}

	Matrix4x4& operator*=(const Matrix4x4& right)
	{
		*this = *this * right;
		return *this;
	}
	
	float getDeterminant()
	{
		Vector4D minor, v1, v2, v3;
		float det = 0.0f;
		
		v1 = Vector4D(this->mat[0][0], this->mat[1][0], this->mat[2][0], this->mat[3][0]);
		v2 = Vector4D(this->mat[0][1], this->mat[1][1], this->mat[2][1], this->mat[3][1]);
		v3 = Vector4D(this->mat[0][2], this->mat[1][2], this->mat[2][2], this->mat[3][2]);
		
		minor.cross(v1, v2, v3);
		det = -(this->mat[0][3] * minor.m_x + this->mat[1][3] * minor.m_y + this->mat[2][3] * minor.m_z + this->mat[3][3] * minor.m_w);
		return det;
	}
	
	// Invert the matrix - super slow
	void invert();
	
	void inverse()
	{
		int a = 0, i = 0, j = 0;
		Matrix4x4 out = {};
		Vector4D v = {}, vec[3] = {};
		float det = 0.0f;

		det = this->getDeterminant();
		if (!det) return;
		for (i = 0; i<4; i++)
		{
			for (j = 0; j<4; j++)
			{
				if (j != i)
				{
					a = j;
					if (j > i) a = a - 1;
					vec[a].m_x = (this->mat[j][0]);
					vec[a].m_y = (this->mat[j][1]);
					vec[a].m_z = (this->mat[j][2]);
					vec[a].m_w = (this->mat[j][3]);
				}
			}
			v.cross(vec[0], vec[1], vec[2]);
			out.mat[0][i] = (float)pow(-1.0f, i) * v.m_x / det;
			out.mat[1][i] = (float)pow(-1.0f, i) * v.m_y / det;
			out.mat[2][i] = (float)pow(-1.0f, i) * v.m_z / det;
			out.mat[3][i] = (float)pow(-1.0f, i) * v.m_w / det;
		}
		this->setMatrix(out);
	}
	
	void setMatrix(const Matrix4x4& matrix)
	{
		::memcpy(mat, matrix.mat, sizeof(mat));
	}
	
	Vector3D getTranslation() const
	{
		return Vector3D(mat[3][0], mat[3][1], mat[3][2]);
	}

	Vector3D getXAxis() const
	{
		return Vector3D::normalize(Vector3D(mat[0][0], mat[0][1], mat[0][2]));
	}

	Vector3D getYAxis() const
	{
		return Vector3D::normalize(Vector3D(mat[1][0], mat[1][1], mat[1][2]));
	}

	Vector3D getZAxis() const
	{
		return Vector3D::normalize(Vector3D(mat[2][0], mat[2][1], mat[2][2]));
	}

	Vector3D getScale() const
	{
		Vector3D retVal;
		retVal.m_x = Vector3D(mat[0][0], mat[0][1], mat[0][2]).length();
		retVal.m_y = Vector3D(mat[1][0], mat[1][1], mat[1][2]).length();
		retVal.m_z = Vector3D(mat[2][0], mat[2][1], mat[2][2]).length();
		return retVal;
	}

	static Matrix4x4 createScale(float xScale, float yScale, float zScale)
	{
		float temp[4][4] =
		{
			{ xScale, 0.0f, 0.0f, 0.0f },
			{ 0.0f, yScale, 0.0f, 0.0f },
			{ 0.0f, 0.0f, zScale, 0.0f },
			{ 0.0f, 0.0f, 0.0f, 1.0f }
		};
		
		return Matrix4x4(temp);
	}

	static Matrix4x4 createScale(const Vector3D& scaleVector)
	{
		return createScale(scaleVector.m_x, scaleVector.m_y, scaleVector.m_z);
	}

	static Matrix4x4 createScale(float scale)
	{
		return createScale(scale, scale, scale);
	}

	static Matrix4x4 createRotationX(float theta)
	{
		float temp[4][4] =
		{
			{ 1.0f, 0.0f, 0.0f , 0.0f },
			{ 0.0f, Maths::cos(theta), Maths::sin(theta), 0.0f },
			{ 0.0f, Maths::sin(theta), Maths::cos(theta), 0.0f },
			{ 0.0f, 0.0f, 0.0f, 1.0f },
		};
		return Matrix4x4(temp);
	}

	static Matrix4x4 createRotationY(float theta)
	{
		float temp[4][4] =
		{
			{ Maths::cos(theta), 0.0f, -Maths::sin(theta), 0.0f },
			{ 0.0f, 1.0f, 0.0f, 0.0f },
			{ Maths::sin(theta), 0.0f, Maths::cos(theta), 0.0f },
			{ 0.0f, 0.0f, 0.0f, 1.0f },
		};
		return Matrix4x4(temp);
	}

	static Matrix4x4 createRotationZ(float theta)
	{
		float temp[4][4] =
		{
			{ Maths::cos(theta), Maths::sin(theta), 0.0f, 0.0f },
			{ -Maths::sin(theta), Maths::cos(theta), 0.0f, 0.0f },
			{ 0.0f, 0.0f, 1.0f, 0.0f },
			{ 0.0f, 0.0f, 0.0f, 1.0f },
		};
		return Matrix4x4(temp);
	}

	static Matrix4x4 createTranslation(const Vector3D& trans)
	{
		float temp[4][4] =
		{
			{ 1.0f, 0.0f, 0.0f, 0.0f },
			{ 0.0f, 1.0f, 0.0f, 0.0f },
			{ 0.0f, 0.0f, 1.0f, 0.0f },
			{ trans.m_x, trans.m_y, trans.m_z, 1.0f }
		};
		return Matrix4x4(temp);
	}

	static Matrix4x4 createSimpleViewProj(float width, float height)
	{
		float temp[4][4] =
		{
			{ 2.0f / width, 0.0f, 0.0f, 0.0f },
			{ 0.0f, 2.0f / height, 0.0f, 0.0f },
			{ 0.0f, 0.0f, 1.0f, 0.0f },
			{ 0.0f, 0.0f, 1.0f, 1.0f }
		};
		return Matrix4x4(temp);
	}

	static Matrix4x4 createFromQuaternion(const Quaternion& q)
	{
		float mat[4][4];

		mat[0][0] = 1.0f - 2.0f * q.m_y * q.m_y - 2.0f * q.m_z * q.m_z;
		mat[0][1] = 2.0f * q.m_x * q.m_y + 2.0f * q.w * q.m_z;
		mat[0][2] = 2.0f * q.m_x * q.m_z - 2.0f * q.w * q.m_y;
		mat[0][3] = 0.0f;

		mat[1][0] = 2.0f * q.m_x * q.m_y - 2.0f * q.w * q.m_z;
		mat[1][1] = 1.0f - 2.0f * q.m_x * q.m_x - 2.0f * q.m_z * q.m_z;
		mat[1][2] = 2.0f * q.m_y * q.m_z + 2.0f * q.w * q.m_x;
		mat[1][3] = 0.0f;

		mat[2][0] = 2.0f * q.m_x * q.m_z + 2.0f * q.w * q.m_y;
		mat[2][1] = 2.0f * q.m_y * q.m_z - 2.0f * q.w * q.m_x;
		mat[2][2] = 1.0f - 2.0f * q.m_x * q.m_x - 2.0f * q.m_y * q.m_y;
		mat[2][3] = 0.0f;

		mat[3][0] = 0.0f;
		mat[3][1] = 0.0f;
		mat[3][2] = 0.0f;
		mat[3][3] = 1.0f;

		return Matrix4x4(mat);
	}

	static Matrix4x4 createLookAt(const Vector3D& eye, const Vector3D& target, const Vector3D& up)
	{
		Vector3D zaxis = Vector3D::normalize(target - eye);
		Vector3D xaxis = Vector3D::normalize(Vector3D::cross(up, zaxis));
		Vector3D yaxis = Vector3D::normalize(Vector3D::cross(zaxis, xaxis));
		Vector3D trans;
		trans.m_x = -Vector3D::dot(xaxis, eye);
		trans.m_y = -Vector3D::dot(yaxis, eye);
		trans.m_z = -Vector3D::dot(zaxis, eye);

		float temp[4][4] =
		{
			{ xaxis.m_x, yaxis.m_x, zaxis.m_x, 0.0f },
			{ xaxis.m_y, yaxis.m_y, zaxis.m_y, 0.0f },
			{ xaxis.m_z, yaxis.m_z, zaxis.m_z, 0.0f },
			{ trans.m_x, trans.m_y, trans.m_z, 1.0f }
		};
		return Matrix4x4(temp);
	}
	
/*
	static Matrix4x4 createOrtho(float width, float height, float near, float far)
	{
		float temp[4][4] =
		{
			{ 2.0f / width, 0.0f, 0.0f, 0.0f },
			{ 0.0f, 2.0f / height, 0.0f, 0.0f },
			{ 0.0f, 0.0f, 1.0f / (far - near), 0.0f },
			{ 0.0f, 0.0f, near / (near - far), 1.0f }
		};
		return Matrix4x4(temp);
	}

	static Matrix4x4 createPerspectiveFOV(float fovY, float width, float height, float near, float far)
	{
		float yScale = Maths::cot(fovY / 2.0f);
		float xScale = yScale * height / width;
		
		float temp[4][4] =
		{
			{ xScale, 0.0f, 0.0f, 0.0f },
			{ 0.0f, yScale, 0.0f, 0.0f },
			{ 0.0f, 0.0f, far / (far - near), 1.0f },
			{ 0.0f, 0.0f, -near * far / (far - near), 0.0f }
		};
		
		return Matrix4x4(temp);
	}
*/
	
	void setPerspectiveFovLH(float fov, float aspect, float znear, float zfar)
	{
		float yscale = 1.0f / (float)tan(fov / 2.0f);
		float xscale = yscale / aspect;
		mat[0][0] = xscale;
		mat[1][1] = yscale;
		mat[2][2] = zfar / (zfar - znear);
		mat[2][3] = 1.0f;
		mat[3][2] = (-znear * zfar)/ (zfar - znear);
		mat[3][3] = 0.0f;
	}
	
	void setOrthoLH(float width,float height,float near_plane, float far_plane)
	{
		setIdentity();
		mat[0][0] = 2.0f / width;
		mat[1][1] = 2.0f / height;
		mat[2][2] = 1.0f / (far_plane - near_plane);
		mat[3][2] = -(near_plane / (far_plane - near_plane));
	}
	
	static const Matrix4x4 identity;
};