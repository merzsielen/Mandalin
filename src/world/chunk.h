#ifndef CHUNK_H
#define CHUNK_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "hex.h"

namespace Mandalin
{
	/*-------------------------------------------------*/
	/* Vertex                                          */
	/*-------------------------------------------------*/
	struct Vertex
	{
		// Spatial Coordinates
		float			x;
		float			y;
		float			z;

		// Color
		float			r;
		float			g;
		float			b;
		float			a;
	};

	/*-------------------------------------------------*/
	/* Triangle                                        */
	/*-------------------------------------------------*/
	struct Triangle
	{
		Vertex	a;
		Vertex	b;
		Vertex	c;
	};

	/*-------------------------------------------------*/
	/* Chunk                                           */
	/*-------------------------------------------------*/
	/*
		Chunks exist solely to minimize the number of
		tiles we send to the renderer each frame. They
		will *only* contain the triangle data of hexes.

		Given that each hex may have up to 18 associated
		triangles (the six making up its face and the
		twelve making up its sides, since each hex is
		really more like a hex-shaped column extending
		down into the planet), we need to limit the
		number of hexes per chunk to around 100 in
		order to avoid going over our ~1,800 triangles-
		per-chunk (soft) limit.

		Each hex will have to include its chunk, its
		index in the chunk, and the number of triangles
		associated with it as the 12 pentagons scattered
		around the planet complicate any cleaner way
		of calculating these things.
	*/
	struct Chunk
	{
		static constexpr unsigned int MAXHEXES = 200;
		static constexpr unsigned int MAXTRIS = 1200;

		unsigned int	index;
		glm::vec3		center;
		float			distance = 0.0f;

		unsigned int	triCount;

		GLuint			vao;
		GLuint			vbo;

		unsigned int	hexCount;
		Hex				hexes[MAXHEXES];
	};

}

#endif