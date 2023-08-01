#ifndef OCEAN_H
#define OCEAN_H

#include "chunk.h"
#include "../util/geometry.h"

namespace Mandalin
{
	struct OceanChunk
	{
		unsigned int	index;
		glm::vec3		center;

		unsigned int	triCount;

		GLuint			vao;
		GLuint			vbo;
	};

	class Ocean
	{
	private:
		/*-----------------------------------------------*/
		/* Chunks */
		/*-----------------------------------------------*/
		std::vector<OceanChunk>		chunks;

	public:
		/*-----------------------------------------------*/
		/* Chunks, Cont. */
		/*-----------------------------------------------*/
		unsigned int				ChunkCount() { return chunks.size(); }
		OceanChunk*					GetChunk(unsigned int i) { return &chunks[i]; }

		/*-----------------------------------------------*/
		/* World Generation */
		/*-----------------------------------------------*/
		Ocean(Polyhedron* polyhedron);
	};
}

#endif