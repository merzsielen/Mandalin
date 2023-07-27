#ifndef CHECKERROR_H
#define CHECKERROR_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Mandalin
{
	void CheckGLError(const char* file, int line);
}

#endif