#include "geometry.h"

#include <corecrt_math_defines.h>
#include <glm/gtx/norm.hpp>
#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <glm/gtx/hash.hpp>

namespace Mandalin
{
	bool CompareDistances(HexNode a, HexNode b)
	{
		glm::vec3 up = glm::vec3(0.0f, 10000.0f, 0.0f);
		glm::vec3 right = glm::vec3(10000.0f, 0.0f, 0.0f);
		glm::vec3 forward = glm::vec3(0.0f, 0.0f, 10000.0f);

		return (glm::distance2(up, a.center) + glm::distance2(right, a.center) + glm::distance2(forward, a.center)) >
			(glm::distance2(up, b.center) + glm::distance2(right, b.center) + glm::distance2(forward, b.center));
	}

	int GetVoronoiStart(int size, std::vector<int> selectedStarts)
	{
		int r = rand() % size;

		bool found = false;

		for (int i = 0; i < selectedStarts.size(); i++)
		{
			if (selectedStarts[i] == r) found = true;
		}

		if (found) return GetVoronoiStart(size, selectedStarts);
		return r;
	}

	std::vector<HexNode> VoronoiSort(std::vector<HexNode> unordered, int desiredCount)
	{
		srand(time(NULL));
		int count = 1 + (unordered.size() / desiredCount);
		std::vector<std::vector<HexNode>> sorted;
		std::vector<int> selectedStarts = {};

		int stride = unordered.size() / count;

		for (int i = 0; i < unordered.size(); i += stride)
		{
			selectedStarts.push_back(i);
			sorted.push_back({ unordered[i] });
			unordered[i].filled = true;
		}

		/*for (int i = 0; i < count; i++)
		{
			int r = GetVoronoiStart(unordered.size(), selectedStarts);
			selectedStarts.push_back(r);
			sorted.push_back({ unordered[r] });
			unordered[r].filled = true;
		}*/

		int filled = sorted.size();

		while (filled < unordered.size())
		{
			// For each floodfill group.
			for (int i = 0; i < sorted.size(); i++)
			{
				std::vector<HexNode> newNodes;

				// For each member of the group.
				for (int j = 0; j < sorted[i].size(); j++)
				{
					// For each neighbor of the member.
					for (int k = 0; k < sorted[i][j].neighbors.size(); k++)
					{
						HexNode* hn = &unordered[sorted[i][j].neighbors[k]];

						if (hn->filled == false)
						{
							hn->filled = true;
							newNodes.push_back(*hn);
							filled++;
						}
					}
				}

				for (int j = 0; j < newNodes.size(); j++) sorted[i].push_back(newNodes[j]);
			}
		}

		std::vector<HexNode> out;
		for (int i = 0; i < sorted.size(); i++)
		{
			for (int j = 0; j < sorted[i].size(); j++)
			{
				out.push_back(sorted[i][j]);
			}
		}
		return out;
	}

	struct Vec3Hash
	{
		size_t operator()(const glm::vec3& v) const
		{
			return std::hash<glm::vec3>()(v);
		}
	};

	void Polyhedron::Subdivide()
	{
		int added = 0;
		std::vector<TriFace> out;
		out.reserve(faces.size() * 4);

		std::unordered_map<glm::vec3, unsigned int, Vec3Hash> vertexMap;

		for (int i = 0; i < vertices.size(); i++)
		{
			vertices[i].sharers.clear();
		}

		for (int i = 0; i < faces.size(); i++)
		{
			/*--------------------------------------------*/
			/* Old Vertices                               */
			/*--------------------------------------------*/
			/*
				First, we grab the indices of the
				old vertices which aren't going to
				change.
			*/
			unsigned int aI = faces[i].a;
			unsigned int bI = faces[i].b;
			unsigned int cI = faces[i].c;

			/*
				Then, we grab the vertices which also
				aren't going to change. However, the
				triangles to which they belong are
				going to change (sadly).
			*/
			PolyVert* a = &vertices[aI];
			PolyVert* b = &vertices[bI];
			PolyVert* c = &vertices[cI];

			/*--------------------------------------------*/
			/* New Vertices                               */
			/*--------------------------------------------*/
			/*
				We need to find out where each new
				vertex will be.
			*/
			unsigned int abI = 0;
			unsigned int bcI = 0;
			unsigned int caI = 0;

			PolyVert midAB = { radius * glm::normalize((a->vertex + b->vertex) / 2.0f), {} };
			PolyVert midBC = { radius * glm::normalize((b->vertex + c->vertex) / 2.0f), {} };
			PolyVert midCA = { radius * glm::normalize((c->vertex + a->vertex) / 2.0f), {} };

			/*--------------------------------------------*/
			/* Find Midpoint Indices                      */
			/*--------------------------------------------*/
			bool foundAB = false;
			bool foundBC = false;
			bool foundCA = false;

			auto itAB = vertexMap.find(midAB.vertex);
			if (itAB == vertexMap.end())
			{
				abI = vertices.size();
				vertexMap[midAB.vertex] = abI;
				vertices.push_back(midAB);
			}
			else
			{
				foundAB = true;
				abI = itAB->second;
			}

			auto itBC = vertexMap.find(midBC.vertex);
			if (itBC == vertexMap.end())
			{
				bcI = vertices.size();
				vertexMap[midBC.vertex] = bcI;
				vertices.push_back(midBC);
			}
			else
			{
				foundBC = true;
				bcI = itBC->second;
			}

			auto itCA = vertexMap.find(midCA.vertex);
			if (itCA == vertexMap.end())
			{
				caI = vertices.size();
				vertexMap[midCA.vertex] = caI;
				vertices.push_back(midCA);
			}
			else
			{
				foundCA = true;
				caI = itCA->second;
			}

			/*--------------------------------------------*/
			/* Finding Triangles                          */
			/*--------------------------------------------*/
			/*
				My brain is a little scattered today
				bear with me.
			*/
			out[added] = { aI, abI, caI };
			out[added + 1] = { abI, bI, bcI };
			out[added + 2] = { caI, bcI, cI };
			out[added + 3] = { abI, bcI, caI };

			//out.push_back({ aI, 0 /* abI */, 0 /* caI */ });
			//out.push_back({ 0 /* abI */, bI, 0 /* bcI */ });
			//out.push_back({ 0 /* caI */, 0 /* bcI */, cI });
			//out.push_back({ 0 /* abI */, 0 /* bcI */, 0 /* caI */ });

			// You could fit my brain in a thimble.
			unsigned int taI = added;
			unsigned int tbI = added + 1;
			unsigned int tcI = added + 2;
			unsigned int tdI = added + 3;
			added += 4;

			TriFace* ta = &out[taI];
			TriFace* tb = &out[tbI];
			TriFace* tc = &out[tcI];
			TriFace* td = &out[tdI];

			/*
				First we're going to add these triangles to
				the vertices we already have indices for.
			*/
			vertices[aI].sharers.push_back(taI);
			vertices[bI].sharers.push_back(tbI);
			vertices[cI].sharers.push_back(tcI);

			/*std::cout << aI << std::endl;
			a->sharers.push_back(taI);
			b->sharers.push_back(tbI);
			c->sharers.push_back(tcI);*/

			/*
				Now we go through and find all the indices
				for the vertices we just added.
			*/
			vertices[abI].sharers.push_back(taI);
			vertices[abI].sharers.push_back(tbI);
			vertices[abI].sharers.push_back(tdI);
			vertices[bcI].sharers.push_back(tbI);
			vertices[bcI].sharers.push_back(tcI);
			vertices[bcI].sharers.push_back(tdI);
			vertices[caI].sharers.push_back(tcI);
			vertices[caI].sharers.push_back(tdI);
			vertices[caI].sharers.push_back(taI);
		}

		faces.clear();	// I don't think this is strictly necessary.
						// But also, what the heck.
		for (int i = 0; i < added; i++) faces.push_back(out[i]);
	}

	Polyhedron::Polyhedron(int worldSize)
	{
		/*
			In a better world, I wouldn't assume the only thing we're
			going to use this for is for world-generation, but alas
			this is not a better world.
		*/

		float x = 0.525731112119133606f;
		float z = 0.850650808352039932f;
		float n = 0.0f;

		vertices =
		{
			{ { -x, n, z }, {} }, { { x, n, z }, {} }, { { -x, n, -z }, {} }, { { x, n, -z }, {} },
			{ { n, z, x }, {} }, { { n, z, -x }, {} }, { { n, -z, x }, {} }, { { n, -z, -x }, {} },
			{ { z, x, n }, {} }, { { -z, x, n }, {} }, { { z, -x, n }, {} }, { { -z, -x, n }, {} }
		};

		faces =
		{
			{ 0, 4, 1 }, { 0, 9, 4 }, { 9, 5, 4 }, { 4, 5, 8 }, { 4, 8, 1 },
			{ 8, 10, 1 }, { 8, 3, 10 }, { 5, 3, 8 }, { 5, 2, 3 }, { 2, 7, 3 },
			{ 7, 10, 3 },{ 7, 6, 10 }, { 7, 11, 6 }, { 11, 0, 6 }, { 0, 1, 6 },
			{ 6, 1, 10 }, { 9, 0, 11 }, { 9, 11, 2 }, { 9, 2, 5 }, { 7, 2, 11 }
		};

		for (int i = 0; i < vertices.size(); i++)
		{
			vertices[i].vertex = radius * glm::normalize(vertices[i].vertex);

			// I know. I don't care.
			for (int j = 0; j < faces.size(); j++)
			{
				if (faces[j].a == i || faces[j].b == i || faces[j].c == i) vertices[i].sharers.push_back(j);
			}
		}
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