#pragma once

#ifdef VCT_DEBUG
    #define GL_CHECK_ERROR() CheckGLError();
#else
    #define GL_CHECK_ERROR()
#endif // VCT_DEBUG

void CheckGLError();