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
	/* Face */
	/*-----------------------------------------------*/
	struct Face
	{
		std::vector<unsigned int> vertices;
	};

	/*-----------------------------------------------*/
	/* Edge */
	/*-----------------------------------------------*/
	struct Edge
	{
		unsigned int a;
		unsigned int b;
	};

	/*-----------------------------------------------*/
	/* Polyhedrons */
	/*-----------------------------------------------*/
	struct Polyhedron
	{
		/*
			Verts, Faces, & Edges
		*/
		std::vector<glm::vec3>	vertices;
		std::vector<Face>		faces;
		std::vector<Edge>		edges;

		/*
			Center & Midpoint
		*/
		glm::vec3 Center(Face face)
		{
			glm::vec3 sum = { 0.0f, 0.0f, 0.0f };
			for (int i = 0; i < face.vertices.size(); i++) sum += vertices[face.vertices[i]];
			return sum / (float)face.vertices.size();
		}

		glm::vec3 Midpoint(Edge edge)
		{
			return (vertices[edge.a] + vertices[edge.b]) / 2.0f;
		}

		/*
			Operator Functions
		*/
		void					Triakis(float apex);
		void					Dual();
		void					Truncate(float kisApex);

		/*
			Constructor
		*/
		Polyhedron();
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
