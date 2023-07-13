#ifndef GEOMETRY_H
#define GEOMETRY_H

/*
	geometry.h

	Structs and functions for holding and manipulating
	spacial data.
*/

#include <vector>
#include <glm/glm.hpp>

namespace Mandalin
{
	/*-----------------------------------------------*/
	/* Temporary Node (for World Generation) */
	/*-----------------------------------------------*/
	struct TempNode
	{
		glm::vec3					vertex;
		unsigned int				id;

		std::vector<unsigned int>	neighborIDs;
	};

	/*-----------------------------------------------*/
	/* TriFace */
	/*-----------------------------------------------*/
	struct TriFace
	{
		glm::vec3		vertices[3];
		unsigned int	vertIDS[3];

		void Normalize(float n)
		{
			vertices[0] = n * glm::normalize(vertices[0]);
			vertices[1] = n * glm::normalize(vertices[1]);
			vertices[2] = n * glm::normalize(vertices[2]);
		}
	};

	/*-----------------------------------------------*/
	/* Polyhedron */
	/*-----------------------------------------------*/
	struct Polyhedron
	{
		unsigned int			vertices;
		std::vector<TriFace>	faces;

		void					Subdivide();
		Polyhedron(int worldSize);
	};

	/*-----------------------------------------------*/
	/* Rays */
	/*-----------------------------------------------*/
	struct Ray
	{
		glm::vec3 position;
		glm::vec3 direction;
	};

	/*-----------------------------------------------*/
	/* Vector Functions */
	/*-----------------------------------------------*/
	glm::vec3 Lerp(glm::vec3 a, glm::vec3 b, float step);

	/*-----------------------------------------------*/
	/* Quaternions */
	/*-----------------------------------------------*/
	struct Quaternion
	{
		float w;
		float x;
		float y;
		float z;

		Quaternion operator*(const Quaternion& rhs) const noexcept
		{
			Quaternion q = { 0, 0, 0, 0 };
			q.w = (this->w * rhs.w - this->x * rhs.x - this->y * rhs.y - this->z * rhs.z);
			q.x = (this->w * rhs.x + this->x * rhs.w + this->y * rhs.z - this->z * rhs.y);
			q.y = (this->w * rhs.y - this->x * rhs.z + this->y * rhs.w + this->z * rhs.x);
			q.z = (this->w * rhs.z + this->x * rhs.y - this->y * rhs.x + this->z * rhs.w);
			return q;
		}

		bool operator==(const Quaternion& rhs) const noexcept
		{
			return ((this->w == rhs.w) && (this->x == rhs.x) && (this->y == rhs.y) && (this->z == rhs.z));
		}

		Quaternion operator-() const noexcept
		{
			return Quaternion{ -this->w, -this->x, -this->y, -this->z };
		}
	};

	/*-----------------------------------------------*/
	/* Quaternion Functions */
	/*-----------------------------------------------*/
	Quaternion Slerp(Quaternion q, Quaternion r, float step);
	float QuaternionDistance(Quaternion l, Quaternion r);

	void NormalizeQuaternion(Quaternion& q);
	Quaternion EulerToQuaternion(glm::vec3 e);
	glm::vec3 QuaternionToEuler(Quaternion q);

	glm::vec3 RotateRelative(glm::vec3 relative, glm::vec3 position, Quaternion q);
	glm::vec3 Rotate(glm::vec3 position, Quaternion q);

	/*-----------------------------------------------*/
	/* Bezier Curves */
	/*-----------------------------------------------*/
	struct BezierCurve
	{
		std::vector<glm::vec3> points;

		glm::vec3 GetPoint(float t);
	};

	/*-----------------------------------------------*/
	/* Bezier Curves for Quaternions */
	/*-----------------------------------------------*/
	struct BezierQuaternion
	{
		std::vector<Quaternion> rotations;

		Quaternion GetQuaternion(float t);
	};
}

#endif
