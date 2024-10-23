#pragma once

#include <memory>
#include "..\Prerequisites.h"
#include "Vec3.h"
#include "Vec4.h"

class  Mat4
{
public:
	Mat4()
	{
		setIdentity();
	}

	void setIdentity()
	{
		::memset(m_mat, 0, sizeof(m_mat));
		m_mat[0][0] = 1;
		m_mat[1][1] = 1;
		m_mat[2][2] = 1;
		m_mat[3][3] = 1;
	}

	void setTranslation(const  Vec3& translation)
	{
		m_mat[3][0] = translation.x;
		m_mat[3][1] = translation.y;
		m_mat[3][2] = translation.z;
	}

	void setScale(const  Vec3& scale)
	{
		m_mat[0][0] = (f32)scale.x;
		m_mat[1][1] = (f32)scale.y;
		m_mat[2][2] = (f32)scale.z;
	}

	void setRotationX(f32 x)
	{
		m_mat[1][1] = (f32)cos(x);
		m_mat[1][2] = (f32)sin(x);
		m_mat[2][1] = (f32)-sin(x);
		m_mat[2][2] = (f32)cos(x);
	}

	void setRotationY(f32 y)
	{
		m_mat[0][0] = (f32)cos(y);
		m_mat[0][2] = (f32)-sin(y);
		m_mat[2][0] = (f32)sin(y);
		m_mat[2][2] = (f32)cos(y);
	}

	void setRotationZ(f32 z)
	{
		m_mat[0][0] = (f32)cos(z);
		m_mat[0][1] = (f32)sin(z);
		m_mat[1][0] = (f32)-sin(z);
		m_mat[1][1] = (f32)cos(z);
	}

	f32 getDeterminant()
	{
		Vec4 minor;
		f32 det = 0.0f;

		Vec4 v1 = Vec4(this->m_mat[0][0], this->m_mat[1][0], this->m_mat[2][0], this->m_mat[3][0]);
		Vec4 v2 = Vec4(this->m_mat[0][1], this->m_mat[1][1], this->m_mat[2][1], this->m_mat[3][1]);
		Vec4 v3 = Vec4(this->m_mat[0][2], this->m_mat[1][2], this->m_mat[2][2], this->m_mat[3][2]);
		
		minor.cross(v1, v2, v3);
		det = -(this->m_mat[0][3] * minor.x + this->m_mat[1][3] * minor.y + this->m_mat[2][3] * minor.z + this->m_mat[3][3] * minor.w);
		
		return det;
	}

	void inverse()
	{
		i32 a = 0, i = 0, j = 0;
		Mat4 out = {};
		Vec4 v = {}, vec[3] = {};
		f32 det = 0.0f;

		det = this->getDeterminant();
		if (!det) return;
		
		for (i = 0; i < 4; i++)
		{
			for (j = 0; j < 4; j++)
			{
				if (j != i)
				{
					a = j;
					if (j > i)
					{
						a = a - 1;
					}
					
					vec[a].x = (this->m_mat[j][0]);
					vec[a].y = (this->m_mat[j][1]);
					vec[a].z = (this->m_mat[j][2]);
					vec[a].w = (this->m_mat[j][3]);
				}
			}
			v.cross(vec[0], vec[1], vec[2]);

			out.m_mat[0][i] = static_cast<f32>(pow(-1.0f, i)) * v.x / det;
			out.m_mat[1][i] = static_cast<f32>(pow(-1.0f, i)) * v.y / det;
			out.m_mat[2][i] = static_cast<f32>(pow(-1.0f, i)) * v.z / det;
			out.m_mat[3][i] = static_cast<f32>(pow(-1.0f, i)) * v.w / det;
		}

		this->setMatrix(out);
	}
	
	void operator *=(const  Mat4& m_matrix)
	{
		Mat4 out;
		for (i32 i = 0; i < 4; i++)
		{
			for (i32 j = 0; j < 4; j++)
			{
				out.m_mat[i][j] =
					m_mat[i][0] * m_matrix.m_mat[0][j] + m_mat[i][1] * m_matrix.m_mat[1][j] +
					m_mat[i][2] * m_matrix.m_mat[2][j] + m_mat[i][3] * m_matrix.m_mat[3][j];
			}
		}
		setMatrix(out);
	}

	void setMatrix(const  Mat4& m_matrix)
	{
		::memcpy(m_mat, m_matrix.m_mat, sizeof(m_mat));
	}

	Vec3 getRightwardDirection()
	{
		return {m_mat[0][0], m_mat[0][1], m_mat[0][2]};
	}
	Vec3 getUpDirection()
	{
		return {m_mat[1][0], m_mat[1][1], m_mat[1][2]};
	}
	Vec3 getForwardDirection()
	{
		return {m_mat[2][0], m_mat[2][1], m_mat[2][2]};
	}

	Vec3 getTranslation()
	{
		return {m_mat[3][0], m_mat[3][1], m_mat[3][2]};
	}

	void setPerspectiveFovLH(f32 fov, f32 aspect, f32 znear, f32 zfar)
	{
		f32 yscale = 1.0f / (f32)tan(fov / 2.0f);
		f32 xscale = yscale / aspect;
		
		m_mat[0][0] = xscale;
		m_mat[1][1] = yscale;
		
		m_mat[2][2] = zfar / (zfar - znear);
		m_mat[2][3] = 1.0f;
		
		m_mat[3][2] = (-znear * zfar) / (zfar - znear);
		m_mat[3][3] = 0.0f;
	}
	
	void setOrthoLH(f32 width, f32 height, f32 near_plane, f32 far_plane)
	{
		setIdentity();
		
		m_mat[0][0] = 2.0f / width;
		m_mat[1][1] = 2.0f / height;
		m_mat[2][2] = 1.0f / (far_plane - near_plane);
		m_mat[3][2] = -(near_plane / (far_plane - near_plane));
	}
	
	f32 m_mat[4][4] = {};
};
