#pragma once

#include <math.h>
#include <cmath>
#include <DirectXMath.h>
#include <algorithm>

#include "../SDK/Vector.h"
#include "../SDK/QAngle.h"
#include "../SDK/VMatrix.h"

#define M_PI 3.14159265358979323846f

//both pasta from CSGOSimple
#define RAD2DEG(x) DirectX::XMConvertToDegrees(x)
#define DEG2RAD(x) DirectX::XMConvertToRadians(x)

#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))

namespace Math {

	QAngle DirectionToAngle(Vector direction);
	void VectorTransform(const Vector& in1, const matrix3x4_t& in2, Vector& out);
	void AngleVectors(const QAngle& angles, Vector& forward);

}