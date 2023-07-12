#include "geometry.h"

#include <corecrt_math_defines.h>
#include <glm/gtx/norm.hpp>
#include <algorithm>
#include <iostream>

namespace Mandalin
{
	std::vector<TriFace> Subdivide(std::vector<TriFace> in)
	{
		std::vector<TriFace> out;

		for (int i = 0; i < in.size(); i++)
		{
			glm::vec3 a = in[i].vertices[0];
			glm::vec3 b = in[i].vertices[1];
			glm::vec3 c = in[i].vertices[2];

			glm::vec3 midAB = (a + b) / 2.0f;
			glm::vec3 midBC = (b + c) / 2.0f;
			glm::vec3 midCA = (c + a) / 2.0f;

			TriFace ta = { { a, midAB, midCA } };
			TriFace tb = { { midAB, b, midBC } };
			TriFace tc = { { midCA, midBC, c } };
			TriFace td = { { midAB, midBC, midCA } };

			ta.Normalize(10.0f);
			tb.Normalize(10.0f);
			tc.Normalize(10.0f);
			td.Normalize(10.0f);

			out.insert(out.end(), { ta, tb, tc, td });
		}

		return out;
	}

	std::vector<TriFace> Icosahedron()
	{
		float x = 0.525731112119133606f;
		float z = 0.850650808352039932f;
		float n = 0.0f;

		std::vector<glm::vec3> vertices =
		{
			{ -x, n, z }, { x, n, z }, { -x, n, -z }, { x, n, -z },
			{ n, z, x }, { n, z, -x }, { n, -z, x }, { n, -z, -x },
			{ z, x, n }, { -z, x, n }, { z, -x, n }, { -z, -x, n }
		};

		std::vector<std::vector<unsigned int>> faces =
		{
			{{ 0, 4, 1 }}, {{ 0, 9, 4 }}, {{ 9, 5, 4 }}, {{ 4, 5, 8 }}, {{ 4, 8, 1 }},
			{{ 8, 10, 1 }}, {{ 8, 3, 10 }}, {{ 5, 3, 8 }}, {{ 5, 2, 3 }}, {{ 2, 7, 3 }},
			{{ 7, 10, 3 }},{{ 7, 6, 10 }}, {{ 7, 11, 6 }}, {{ 11, 0, 6 }}, {{ 0, 1, 6 }},
			{{ 6, 1, 10 }}, {{ 9, 0, 11 }}, {{ 9, 11, 2 }}, {{ 9, 2, 5 }}, {{ 7, 2, 11 }}
		};

		std::vector<TriFace> out;

		for (int i = 0; i < faces.size(); i++)
		{
			out.push_back({ vertices[faces[i][0]], vertices[faces[i][1]], vertices[faces[i][2]] });
		}
		
		return out;
	}

	/*-----------------------------------------------*/
	/* Vector Functions */
	/*-----------------------------------------------*/
	glm::vec3 Lerp(glm::vec3 a, glm::vec3 b, float step)
	{
		return (a * (1.0f - step) + (b * step));
	}

	/*-----------------------------------------------*/
	/* Quaternion Functions */
	/*-----------------------------------------------*/
	Quaternion Slerp(Quaternion l, Quaternion r, float step)
	{
		Quaternion q = { 0, 0, 0, 0 };

		float dot = l.x * r.x + l.y * r.y + l.z * r.z + l.w * r.w;
		float theta, st, sut, sout, coeff1, coeff2;

		step /= 2.0f;

		theta = (float)acos(dot);
		if (theta < 0.0) theta = -theta;

		st = (float)sin(theta);
		sut = (float)sin(step * theta);
		sout = (float)sin((1 - step) * theta);
		coeff1 = sout / st;
		coeff2 = sut / st;

		q.x = coeff1 * l.x + coeff2 * r.x;
		q.y = coeff1 * l.y + coeff2 * r.y;
		q.z = coeff1 * l.z + coeff2 * r.z;
		q.w = coeff1 * l.w + coeff2 * r.w;

		NormalizeQuaternion(q);

		if (isnan(q.x) || isnan(q.y) || isnan(q.z)) return r;

		if (theta > 2.0) return -q;
		return q;
	}

	float QuaternionDistance(Quaternion l, Quaternion r)
	{
		glm::vec3 ref = { 1.0f, 1.0f, 1.0f };
		glm::vec3 ori = { 0.0f, 0.0f, 0.0f };
		glm::vec3 refL = Rotate(ref, l);
		glm::vec3 refR = Rotate(ref, r);

		return glm::length2(refL - refR);
	}

	void NormalizeQuaternion(Quaternion& q)
	{
		float mag = sqrt(q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z);
		q.w /= mag;
		q.x /= mag;
		q.y /= mag;
		q.z /= mag;
	}

	Quaternion EulerToQuaternion(glm::vec3 e)
	{
		Quaternion q = { 0, 0, 0, 0 };
		double cy = cos(e.z * 0.5);
		double sy = sin(e.z * 0.5);
		double cp = cos(e.y * 0.5);
		double sp = sin(e.y * 0.5);
		double cr = cos(e.x * 0.5);
		double sr = sin(e.x * 0.5);

		q.w = (float)(cr * cp * cy + sr * sp * sy);
		q.x = (float)(sr * cp * cy - cr * sp * sy);
		q.y = (float)(cr * sp * cy + sr * cp * sy);
		q.z = (float)(cr * cp * sy - sr * sp * cy);

		NormalizeQuaternion(q);

		return q;
	}

	glm::vec3 QuaternionToEuler(Quaternion q)
	{
		glm::vec3 e = { 0, 0, 0 };
		double sinrCosp = 2 * (q.w * q.x + q.y * q.z);
		double cosrCosp = 1 - 2 * (q.x * q.x + q.y * q.y);
		e.x = (float)std::atan2(sinrCosp, cosrCosp);

		double sinp = 2 * (q.w * q.y - q.z * q.x);
		if (std::abs(sinp) >= 1)
		{
			e.y = (float)std::copysign(M_PI / 2, sinp);
		}
		else
		{
			e.y = (float)std::asin(sinp);
		}

		double sinyCosp = 2 * (q.w * q.z + q.x * q.y);
		double cosyCosp = 1 - 2 * (q.y, q.y + q.z * q.z);
		e.z = (float)std::atan2(sinyCosp, cosyCosp);

		return e;
	}


	glm::vec3 Rotate(glm::vec3 position, Quaternion q)
	{
		glm::mat3x3 mat = glm::mat3x3(1 - 2 * (q.y * q.y + q.z * q.z), 2 * (q.x * q.y - q.z * q.w), 2 * (q.x * q.z + q.y * q.w),
			2 * (q.x * q.y + q.z * q.w), 1 - 2 * (q.x * q.x + q.z * q.z), 2 * (q.y * q.z - q.x * q.w),
			2 * (q.x * q.z - q.y * q.w), 2 * (q.y * q.z + q.x * q.w), 1 - 2 * (q.x * q.x + q.y * q.y));

		return position * mat;
	}

	glm::vec3 RotateRelative(glm::vec3 relative, glm::vec3 position, Quaternion q)
	{
		return Rotate(relative - position, q) + relative;
	}

	/*-----------------------------------------------*/
	/* Bezier Functions */
	/*-----------------------------------------------*/

	glm::vec3 BezierCurve::GetPoint(float t)
	{
		std::vector<glm::vec3> ps = points;
		std::vector<glm::vec3> tmp;

		for (int i = 0; i < points.size(); i++)
		{
			for (int j = 0; j < ps.size() - 1; j++)
			{
				tmp.push_back(Lerp(ps[j], ps[static_cast<std::vector<glm::vec3, std::allocator<glm::vec3>>::size_type>(j) + 1], t));
			}

			ps = tmp;
			tmp.clear();

			if (ps.size() == 1)
			{
				return ps[0];
			}
		}

		return glm::vec3(0.0f, 0.0f, 0.0f);
	}

	Quaternion BezierQuaternion::GetQuaternion(float t)
	{
		std::vector<Quaternion> qs = rotations;
		std::vector<Quaternion> tmp;

		for (int i = 0; i < rotations.size(); i++)
		{
			for (int j = 0; j < qs.size() - 1; j++)
			{
				tmp.push_back(Slerp(qs[j], qs[j + 1], t));
			}

			qs = tmp;
			tmp.clear();

			if (qs.size() == 1)
			{
				return qs[0];
			}
		}

		return { 1.0f, 0.0f, 0.0f, 0.0f };
	}
}