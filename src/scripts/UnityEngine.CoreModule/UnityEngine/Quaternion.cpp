#include "../UnityEngine.CoreModule.hpp"
#include "../../ScriptInternal.hpp"
#include "Quaternion.hpp"

namespace
{
	Il2CppMethodPointer Quaternion_AngleAxis_Injected_addr = nullptr;

	Il2CppMethodPointer Quaternion_FromToRotation_Injected_addr = nullptr;

	Il2CppMethodPointer Quaternion_Inverse_Injected_addr = nullptr;

	Il2CppMethodPointer Quaternion_Slerp_Injected_addr = nullptr;

	Il2CppMethodPointer Quaternion_Lerp_Injected_addr = nullptr;

	Il2CppMethodPointer Quaternion_Internal_FromEulerRad_Injected_addr = nullptr;

	Il2CppMethodPointer Quaternion_Internal_ToEulerRad_Injected_addr = nullptr;

	constexpr float DEG_2_RAD = 3.1415926535f / 180.0f;
}

static void InitAddress()
{
	Quaternion_AngleAxis_Injected_addr = il2cpp_resolve_icall("UnityEngine.Quaternion::AngleAxis_Injected");
	Quaternion_FromToRotation_Injected_addr = il2cpp_resolve_icall("UnityEngine.Quaternion::FromToRotation_Injected");
	Quaternion_Inverse_Injected_addr = il2cpp_resolve_icall("UnityEngine.Quaternion::Inverse_Injected");
	Quaternion_Slerp_Injected_addr = il2cpp_resolve_icall("UnityEngine.Quaternion::Slerp_Injected");
	Quaternion_Lerp_Injected_addr = il2cpp_resolve_icall("UnityEngine.Quaternion::Lerp_Injected");
	Quaternion_Internal_FromEulerRad_Injected_addr = il2cpp_resolve_icall("UnityEngine.Quaternion::Internal_FromEulerRad_Injected");
	Quaternion_Internal_ToEulerRad_Injected_addr = il2cpp_resolve_icall("UnityEngine.Quaternion::Internal_ToEulerRad_Injected");
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
}

namespace UnityEngine
{
	Quaternion Quaternion::AngleAxis(float angle, Vector3 axis)
	{
		float lengthSq = axis.x * axis.x + axis.y * axis.y + axis.z * axis.z;

		if (lengthSq < 1e-6f)
		{
			return { 0.0f, 0.0f, 0.0f, 1.0f };
		}

		float length = sqrt(lengthSq);
		float invLength = 1.0f / length;
		float normX = axis.x * invLength;
		float normY = axis.y * invLength;
		float normZ = axis.z * invLength;

		float halfAngleRad = (angle * DEG_2_RAD) * 0.5f;

		float sinHalf = sin(halfAngleRad);
		float cosHalf = cos(halfAngleRad);

		return { normX * sinHalf, normY * sinHalf, normZ * sinHalf, cosHalf };
	}

	Quaternion Quaternion::FromToRotation(Vector3 fromDirection, Vector3 toDirection)
	{
		float l1 = sqrt(fromDirection.x * fromDirection.x + fromDirection.y * fromDirection.y + fromDirection.z * fromDirection.z);
		float l2 = sqrt(toDirection.x * toDirection.x + toDirection.y * toDirection.y + toDirection.z * toDirection.z);

		if (l1 < 1e-6f || l2 < 1e-6f)
		{
			return { 0.0f, 0.0f, 0.0f, 1.0f };
		}

		fromDirection.x /= l1; fromDirection.y /= l1; fromDirection.z /= l1;
		toDirection.x /= l2; toDirection.y /= l2; toDirection.z /= l2;

		float dot = fromDirection.x * toDirection.x + fromDirection.y * toDirection.y + fromDirection.z * toDirection.z;

		if (dot >= 0.99999f)
		{
			return { 0.0f, 0.0f, 0.0f, 1.0f };
		}

		if (dot <= -0.99999f)
		{
			Vector3 axis = { 1.0f, 0.0f, 0.0f };
			if (abs(fromDirection.x) > 0.8f)
			{
				axis = { 0.0f, 1.0f, 0.0f };
			}

			Vector3 cross =
			{
				fromDirection.y * axis.z - fromDirection.z * axis.y,
				fromDirection.z * axis.x - fromDirection.x * axis.z,
				fromDirection.x * axis.y - fromDirection.y * axis.x
			};

			float l = sqrt(cross.x * cross.x + cross.y * cross.y + cross.z * cross.z);
			return { cross.x / l, cross.y / l, cross.z / l, 0.0f };
		}

		Vector3 cross =
		{
			fromDirection.y * toDirection.z - fromDirection.z * toDirection.y,
			fromDirection.z * toDirection.x - fromDirection.x * toDirection.z,
			fromDirection.x * toDirection.y - fromDirection.y * toDirection.x
		};

		Quaternion q = { cross.x, cross.y, cross.z, 1.0f + dot };

		float ql = sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
		return { q.x / ql, q.y / ql, q.z / ql, q.w / ql };
	}

	Quaternion Quaternion::Inverse(Quaternion rotation)
	{
		float normSq = rotation.x * rotation.x + rotation.y * rotation.y + rotation.z * rotation.z + rotation.w * rotation.w;
		if (normSq < 1e-6f)
		{
			return { 0.0f, 0.0f, 0.0f, 1.0f };
		}

		return { -rotation.x / normSq, -rotation.y / normSq, -rotation.z / normSq, rotation.w / normSq };
	}

	Quaternion Quaternion::Slerp(Quaternion a, Quaternion b, float t)
	{
		t = max(0.0f, min(1.0f, t));

		float dot = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
		float flip = 1.0f;

		if (dot < 0.0f)
		{
			flip = -1.0f;
			dot = -dot;
		}

		if (dot > 0.9995f)
		{
			float om_t = 1.0f - t;
			Quaternion r = {
				om_t * a.x + t * b.x * flip,
				om_t * a.y + t * b.y * flip,
				om_t * a.z + t * b.z * flip,
				om_t * a.w + t * b.w * flip
			};

			float l = sqrt(r.x * r.x + r.y * r.y + r.z * r.z + r.w * r.w);
			return { r.x / l, r.y / l, r.z / l, r.w / l };
		}

		float theta = acos(dot);
		float sin_theta = sin(theta);

		float wa = sin((1.0f - t) * theta) / sin_theta;
		float wb = sin(t * theta) / sin_theta * flip;

		return
		{
			wa * a.x + wb * b.x,
			wa * a.y + wb * b.y,
			wa * a.z + wb * b.z,
			wa * a.w + wb * b.w
		};
	}

	Quaternion Quaternion::Lerp(Quaternion a, Quaternion b, float t)
	{
		t = max(0.0f, min(1.0f, t));

		float dot = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;

		float flip = 1.0f;

		if (dot < 0.0f)
		{
			flip = -1.0f;
		}

		Quaternion result{};
		float oneMinusT = 1.0f - t;

		result.x = oneMinusT * a.x + t * (b.x * flip);
		result.y = oneMinusT * a.y + t * (b.y * flip);
		result.z = oneMinusT * a.z + t * (b.z * flip);
		result.w = oneMinusT * a.w + t * (b.w * flip);

		float lenSq = result.x * result.x + result.y * result.y + result.z * result.z + result.w * result.w;

		if (lenSq > 1e-6f)
		{
			float invLen = 1.0f / sqrt(lenSq);
			result.x *= invLen;
			result.y *= invLen;
			result.z *= invLen;
			result.w *= invLen;
		}
		else
		{
			result = { 0.0f, 0.0f, 0.0f, 1.0f };
		}

		return result;
	}

	Quaternion Quaternion::Euler(float x, float y, float z)
	{
		float cX = cos(x * DEG_2_RAD * 0.5f);
		float sX = sin(x * DEG_2_RAD * 0.5f);
		float cY = cos(y * DEG_2_RAD * 0.5f);
		float sY = sin(y * DEG_2_RAD * 0.5f);
		float cZ = cos(z * DEG_2_RAD * 0.5f);
		float sZ = sin(z * DEG_2_RAD * 0.5f);

		Quaternion q{};
		q.x = sX * cY * cZ + cX * sY * sZ;
		q.y = cX * sY * cZ - sX * cY * sZ;
		q.z = cX * cY * sZ - sX * sY * cZ;
		q.w = cX * cY * cZ + sX * sY * sZ;
		return q;
	}

	Quaternion Quaternion::Euler(Vector3 euler)
	{
		return Euler(euler.x, euler.y, euler.z);
	}

	Vector3 Quaternion::Internal_ToEulerRad(Quaternion rotation)
	{
		float sinX = 2.0f * (rotation.w * rotation.x - rotation.y * rotation.z);
		Vector3 euler{};

		if (abs(sinX) < 0.99999f)
		{
			euler.x = asin(sinX);
			euler.y = atan2(2.0f * (rotation.w * rotation.y + rotation.x * rotation.z), 1.0f - 2.0f * (rotation.x * rotation.x + rotation.y * rotation.y));
			euler.z = atan2(2.0f * (rotation.w * rotation.z + rotation.x * rotation.y), 1.0f - 2.0f * (rotation.y * rotation.y + rotation.z * rotation.z));
		}
		else
		{
			euler.x = (sinX > 0.0f) ? (3.1415926535f / 2.0f) : (-3.1415926535f / 2.0f);
			euler.y = 2.0f * atan2(rotation.y, rotation.w);
			euler.z = 0.0f;
		}

		return euler;
	}

	Vector3 Quaternion::eulerAngles()
	{
		return Internal_MakePositive(Internal_ToEulerRad(*this) * 57.29578f);
	}
}
