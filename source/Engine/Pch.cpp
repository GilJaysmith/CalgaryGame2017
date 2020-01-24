#include "Engine/Pch.h"

void CheckGLError()
{
	GLenum error = glGetError();
	if (error != GL_NO_ERROR && error != GL_INVALID_OPERATION)
	{
		printf("a");
	}
}
