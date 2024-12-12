#pragma once

#include "Vector3.hpp"
#include "Vector4.hpp"
#include "Quaternion.hpp"

namespace UnityEngine
{
	struct Matrix4x4
	{
	public:
		float m00;
		float m01;
		float m02;
		float m03;
		float m10;
		float m11;
		float m12;
		float m13;
		float m20;
		float m21;
		float m22;
		float m23;
		float m30;
		float m31;
		float m32;
		float m33;

		Matrix4x4(Vector4 column0, Vector4 column1, Vector4 column2, Vector4 column3)
		{
			m00 = column0.x;
			m01 = column1.x;
			m02 = column2.x;
			m03 = column3.x;
			m10 = column0.y;
			m11 = column1.y;
			m12 = column2.y;
			m13 = column3.y;
			m20 = column0.z;
			m21 = column1.z;
			m22 = column2.z;
			m23 = column3.z;
			m30 = column0.w;
			m31 = column1.w;
			m32 = column2.w;
			m33 = column3.w;
		}

		static Matrix4x4 zeroMatrix;
		static Matrix4x4 identityMatrix;
	};
}
