#ifndef OCEAN_H
#define OCEAN_H

#include "chunk.h"
#include "../util/geometry.h"

namespace Mandalin
{
	struct OceanChunk
	{
		static constexpr unsigned int MAXTRIS = 10000;

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
		/* Parameters */
		/*-----------------------------------------------*/
		float						offset = 0.99f;
		glm::vec4					color = glm::vec4(0.13f, 0.55f, 0.69f, 0.5f);

		/*-----------------------------------------------*/
		/* Chunks */
		/*-----------------------------------------------*/
		std::vector<OceanChunk>		chunks;

	public:
		/*-----------------------------------------------*/
		/* Parameters */
		/*-----------------------------------------------*/
		float						GetOffset() { return offset; }

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