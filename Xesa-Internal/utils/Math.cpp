#include "Math.h"

namespace Math {

	QAngle DirectionToAngle(Vector direction)
	{
		float tmp, yaw, pitch;

		if (direction.y == 0 && direction.x == 0)
		{
			yaw = 0;
			if (direction.z > 0)
				pitch = 270;
			else
				pitch = 90;
		}
		else
		{
			yaw = (atan2(direction.y, direction.x) * 180 / M_PI);
			if (yaw < 0)
				yaw += 360;

			tmp = sqrt(direction.x * direction.x + direction.y * direction.y);
			pitch = (atan2(-direction.z, tmp) * 180 / M_PI);
			if (pitch < 0)
				pitch += 360;
		}

		if (pitch > 180.f)
			pitch -= 360.f;

		return QAngle(pitch, yaw, 0.f);
	}

	void VectorTransform(const Vector& in1, const matrix3x4_t& in2, Vector& out)
	{
		out[0] = in1.Dot(in2[0]) + in2[0][3];
		out[1] = in1.Dot(in2[1]) + in2[1][3];
		out[2] = in1.Dot(in2[2]) + in2[2][3];
	}

	void AngleVectors(const QAngle& angles, Vector& forward)
	{
		float	sp, sy, cp, cy;

		DirectX::XMScalarSinCos(&sp, &cp, DEG2RAD(angles[0]));
		DirectX::XMScalarSinCos(&sy, &cy, DEG2RAD(angles[1]));

		forward.x = cp * cy;
		forward.y = cp * sy;
		forward.z = -sp;
	}

}