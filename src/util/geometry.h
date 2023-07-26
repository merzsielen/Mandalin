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
	/* Polyhedron Vertex */
	/*-----------------------------------------------*/
	/*
		Contains a unique vertex and the
		indices of the triangles which share it.
	*/
	struct PolyVert
	{
		glm::vec3					vertex;
		std::vector<unsigned int>	sharers;
	};

	/*-----------------------------------------------*/
	/* TriFace */
	/*-----------------------------------------------*/
	/*
		Just contains the indices of the
		constituent vertices.
	*/
	struct TriFace
	{
		unsigned int			a;
		unsigned int			b;
		unsigned int			c;
	};

	/*-----------------------------------------------*/
	/* Polyhedron */
	/*-----------------------------------------------*/
	struct Polyhedron
	{
		std::vector<PolyVert>	vertices;
		std::vector<TriFace>	faces;

		float					radius = 100.0f;

		void					Subdivide();
		Polyhedron(int worldSize);
	};

	/*-----------------------------------------------*/
	/* Vertex Node (for World Generation) */
	/*-----------------------------------------------*/
	/*
		Each vertex gets made into a hex whose
		neighbors are the hexes made from the
		triangles which share that vertex.

		Only vertex nodes can become pentagons.
	*/
	struct VertNode
	{
		bool			pentagon;
		unsigned int	vertIndex;
		unsigned int	neighborIndices[6];
	};

	/*-----------------------------------------------*/
	/* Triangle Node (for World Generation) */
	/*-----------------------------------------------*/
	/*
		Each triangle gets made into a hex whose
		neighbors are the three hexes made from its
		vertices and the three hexes made from the
		triangles which share two vertices with it.
	*/
	struct TriNode
	{
		unsigned int	triIndex;
		unsigned int	vertNeighborIndices[3];
		unsigned int	triNeighborIndices[3];
	};

	/*-----------------------------------------------*/
	/* Hex Node (for World Generation) */
	/*-----------------------------------------------*/
	struct HexNode
	{
		unsigned int				index;
		glm::vec3					center;
		std::vector<unsigned int>	neighbors;
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
