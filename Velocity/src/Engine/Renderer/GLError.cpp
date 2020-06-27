#include "vctPCH.h"
#include "glad/glad.h"
#include "GLError.h"

void CheckGLError()
{
    int err = glGetError();
    if(err != 0)
    {
        ASSERT(false, "OpenGL Error: {0}", err);
    }
}
