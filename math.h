#pragma once
#include <Windows.h>
#include "globals.hh"	
#include <corecrt_math.h>
#include <intrin.h>
#include "offsets.h"
#define PI 3.1415927f

struct view_matrix_t
{
	float* operator[](int index)
	{
		return matrix[index];
	}

	float matrix[4][4];
};

typedef struct
{
	float matrix[3][4];
} Matrix3x4_T;

struct Vector3
{
	float x, y, z;
};


Vector3 CalcAngle(Vector3& src, Vector3& dst)
{
	//square root func faster than normal func youd use
	const auto sqrtss = [](float in) {
		__m128 reg = _mm_load_ss(&in);
		return _mm_mul_ss(reg, _mm_rsqrt_ss(reg)).m128_f32[0];
	};

	Vector3 angles;

	//getting delta between source and destination vectors
	Vector3 delta;
	delta.x = src.x - dst.x;
	delta.y = src.y - dst.y;
	delta.z = src.z - dst.z;

	float hyp = sqrtss(delta.x * delta.x + delta.y * delta.y);

	angles.x = asinf(delta.z / hyp) * (180.0f / PI);
	angles.y = atanf(delta.y / delta.x) * (180.0f / PI) + !((*(DWORD*)&delta.x) >> 31 & 1) * 180.0f;

	angles.z = 0.0f;

	return angles;
}


Vector3 WorldToScreen(const Vector3 pos, view_matrix_t matrix)
{


	float x_ = matrix[0][0] * pos.x + matrix[0][1] * pos.y + matrix[0][2] * pos.z + matrix[0][3];




	float y_ = matrix[1][0] * pos.x + matrix[1][1] * pos.y + matrix[1][2] * pos.z + matrix[1][3];



	float w_ = matrix[3][0] * pos.x + matrix[3][1] * pos.y + matrix[3][2] * pos.z + matrix[3][3];




	float inv_w = 1.f / w_;
	x_ *= inv_w;
	y_ *= inv_w;


	float x = globals.screenX * .5f;
	float y = globals.screenY * .5f;

	x += 0.5f * x_ * globals.screenX + 0.5f;
	y -= 0.5f * y_ * globals.screenY + 0.5f;

	return { x,y, w_ };
}
