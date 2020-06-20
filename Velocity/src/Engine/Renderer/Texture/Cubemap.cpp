#include "vctPCH.h"
#include "Cubemap.h"

#include "Image.h"

#include "glad/glad.h"
using namespace Vct;

Cubemap::Cubemap(const Path& right, const Path& left, const Path& top, const Path& bottom, const Path& back, const Path& front)
{
    Image img_right     = Image(right);
    Image img_left      = Image(left);
    Image img_top       = Image(top);
    Image img_bottom    = Image(bottom);
    Image img_back      = Image(back);
    Image img_front     = Image(front);

    InitializeCubemap(std::vector<Image*>{&img_right, &img_left, &img_top, &img_bottom, &img_back, &img_front});
}

Cubemap::~Cubemap()
{
    Destroy();
}

void Cubemap::Destroy()
{
    if(RendererId != 0)
    {
        glDeleteTextures(1, &RendererId);
        RendererId = 0;
    }
}

void Cubemap::InitializeCubemap(const std::vector<Image*> faces)
{
    if(faces.size() != 6)
    {
        ASSERT(false, "Expected 6 faces for cubemap");
        return;
    }

    Destroy();

    glGenTextures(1, &RendererId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, RendererId);

    for(int i = 0; i < faces.size(); i++)
    {
        const Image* img = faces[i];
        const int glFormat = img->GetFormat() == ImageFormat::HDR ? GL_RGB16F : GL_RGB;
        const int glType   = img->GetFormat() == ImageFormat::HDR ? GL_FLOAT  : GL_UNSIGNED_BYTE;

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, glFormat, img->GetWidth(), img->GetHeight(), 0, glFormat, glType, img->GetData());
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
