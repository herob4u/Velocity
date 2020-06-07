#include "vctPCH.h"
#include "RenderCommands.h"

/* Experimental */
#include "Texture/Texture.h"
#include "Mesh/MeshFactory.h"
#include "Mesh/Mesh.h"
#include "ShaderProgram.h"

#include "glad/glad.h"

using namespace Vct;

void RenderCommands::ClearBuffers()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void RenderCommands::SetClearColor(const glm::vec3 & color)
{
    glClearColor(color.r, color.g, color.b, 1.f);
}

void RenderCommands::SetClearColor(float r, float g, float b)
{
    glClearColor(r, g, b, 1.f);
}

void RenderCommands::DrawImage(Ref<ShaderProgram> shader, Texture* texture)
{
    if(texture && shader.get())
    {
        static float vertices[] = {-1.f, -1.f, 0.f,     -1.f, 1.f, 0.f
                                , 1.f, -1.f, 0.f,      1.f, 1.f, 0.f };

        static float uvs[] = { 0.f, 0.f,    0.f, 1.f
                            , 1.f, 0.f,    1.f, 1.f };
        
        static uint32_t indices[] = {0, 3, 1,  0, 2, 3};

        static GLuint vao;
        
        shader->enable();

        glCreateVertexArrays(1, &vao);
        glBindVertexArray(vao);
            GLuint vertexBuffer;
            GLuint texCoordBuffer;
            GLuint indexBuffer;

            GLuint posAttribute = shader->getAttribLocation("a_Position");
            GLuint texAttribute = shader->getAttribLocation("a_TexCoord");
            glEnableVertexAttribArray(posAttribute);
            glEnableVertexAttribArray(texAttribute);

            glGenBuffers(1, &vertexBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
            glVertexAttribPointer(posAttribute, 3, GL_FLOAT, false, 0, nullptr);

            glGenBuffers(1, &texCoordBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(uvs), uvs, GL_STATIC_DRAW);
            glVertexAttribPointer(texAttribute, 2, GL_FLOAT, false, 0, nullptr);

            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glGenBuffers(1, &indexBuffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

            texture->Bind();
            glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(uint32_t), GL_UNSIGNED_INT, 0);
            texture->Unbind();


            glDeleteBuffers(1, &vertexBuffer);
            glDeleteBuffers(1, &texCoordBuffer);
            glDeleteBuffers(1, &indexBuffer);

        glBindVertexArray(0);
        glDeleteBuffers(1, &vao);

        shader->disable();
    }
}

void RenderCommands::SetWireframeMode(bool enabled)
{
    enabled ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

