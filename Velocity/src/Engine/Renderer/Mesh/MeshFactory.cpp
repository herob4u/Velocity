#include "vctPCH.h"
#include "MeshFactory.h"

#include "MeshFactory.h"
#include "Mesh.h"

#include "../Types.h"

#include "glm/gtc/constants.hpp"
#include "glm/trigonometric.hpp"

using namespace Vct;

Mesh* MeshFactory::MakeBox(float length, float width, float height)
{
    std::vector<MeshVertex> vertices;
    const int count = 8;
    vertices.reserve(count);

    // Recall Right Handed convention. X = right, Y = up, Z = back
    MeshVertex v;
    //const float x = width / 2.0f;
    //const float y = height / 2.0f;
    //const float z = length / 2.0f;

    // For A2, want a +-1 box extent
    const float x = width;
    const float y = height;
    const float z = length;

    /* BACK FACE */
    // --- 0
    v.Position[0] = -x;
    v.Position[1] = -y;
    v.Position[2] = -z;
    v.Normal[0] = -1 / 3.f;
    v.Normal[1] = -1 / 3.f;
    v.Normal[2] = -1 / 3.f;
    v.UV[0] = 0.f;
    v.UV[1] = 0.f;
    vertices.push_back(v);

    // +-- 1
    v.Position[0] = x;
    v.Position[1] = -y;
    v.Position[2] = -z;
    v.Normal[0] = 1 / 3.f;
    v.Normal[1] = -1 / 3.f;
    v.Normal[2] = -1 / 3.f;
    v.UV[0] = 1.f;
    v.UV[1] = 0.f;
    vertices.push_back(v);

    // -+- 2
    v.Position[0] = -x;
    v.Position[1] = y;
    v.Position[2] = -z;
    v.Normal[0] = -1 / 3.f;
    v.Normal[1] = 1 / 3.f;
    v.Normal[2] = -1 / 3.f;
    v.UV[0] = 0.f;
    v.UV[1] = 1.f;
    vertices.push_back(v);

    // ++- 3
    v.Position[0] = x;
    v.Position[1] = y;
    v.Position[2] = -z;
    v.Normal[0] = 1 / 3.f;
    v.Normal[1] = 1 / 3.f;
    v.Normal[2] = -1 / 3.f;
    v.UV[0] = 1.f;
    v.UV[1] = 1.f;
    vertices.push_back(v);

    /*FRONT FACE*/
    // --+ 4
    v.Position[0] = -x;
    v.Position[1] = -y;
    v.Position[2] = z;
    v.Normal[0] = -1 / 3.f;
    v.Normal[1] = -1 / 3.f;
    v.Normal[2] = 1 / 3.f;
    v.UV[0] = 0.f;
    v.UV[1] = 1.f;
    vertices.push_back(v);

    // +-+ 5
    v.Position[0] = x;
    v.Position[1] = -y;
    v.Position[2] = z;
    v.Normal[0] = 1 / 3.f;
    v.Normal[1] = -1 / 3.f;
    v.Normal[2] = 1 / 3.f;
    v.UV[0] = 1.f;
    v.UV[1] = 1.f;
    vertices.push_back(v);

    // +++ 6
    v.Position[0] = x;
    v.Position[1] = y;
    v.Position[2] = z;
    v.Normal[0] = 1 / 3.f;
    v.Normal[1] = 1 / 3.f;
    v.Normal[2] = 1 / 3.f;
    v.UV[0] = 1.f;
    v.UV[1] = 0.f;
    vertices.push_back(v);

    // -++ 7
    v.Position[0] = -x;
    v.Position[1] = y;
    v.Position[2] = z;
    v.Normal[0] = -1 / 3.f;
    v.Normal[1] = 1 / 3.f;
    v.Normal[2] = 1 / 3.f;
    v.UV[0] = 0.f;
    v.UV[1] = 0.f;
    vertices.push_back(v);

    std::vector<Triangle> indices;
    indices.reserve(8 * 4);

    // Back Face
    //indices.emplace_back(0); indices.emplace_back(1); indices.emplace_back(3);
    //indices.emplace_back(0); indices.emplace_back(3); indices.emplace_back(2);
    indices.emplace_back(0, 1, 3);
    indices.emplace_back(0, 3, 2);

    // Right Face
    //indices.emplace_back(6); indices.emplace_back(1); indices.emplace_back(3);
    //indices.emplace_back(6); indices.emplace_back(5); indices.emplace_back(1);
    indices.emplace_back(6, 1, 3);
    indices.emplace_back(6, 5, 1);

    // Left Face
    //indices.emplace_back(0); indices.emplace_back(7); indices.emplace_back(2);
    //indices.emplace_back(0); indices.emplace_back(4); indices.emplace_back(7);
    indices.emplace_back(0, 7, 2);
    indices.emplace_back(0, 4, 7);

    // Front Face
    //indices.emplace_back(4); indices.emplace_back(5); indices.emplace_back(6);
    //indices.emplace_back(6); indices.emplace_back(7); indices.emplace_back(4);
    indices.emplace_back(4, 5, 6);
    indices.emplace_back(6, 7, 4);

    // Top Face
    //indices.emplace_back(7); indices.emplace_back(6); indices.emplace_back(3);
    //indices.emplace_back(3); indices.emplace_back(2); indices.emplace_back(7);
    indices.emplace_back(7, 6, 3);
    indices.emplace_back(3, 2, 7);

    // Bottom Face
    //indices.emplace_back(0); indices.emplace_back(1); indices.emplace_back(5);
    //indices.emplace_back(5); indices.emplace_back(4); indices.emplace_back(0);
    indices.emplace_back(0, 1, 5);
    indices.emplace_back(5, 4, 0);

    return new StaticMesh(vertices, indices);
}

// @TODO: This is using the Z-axis being up as a convention. Change for Y up for consistency
Mesh* MeshFactory::MakeSphere(float radius, int segments)
{
    if(radius <= 0 || segments <= 0)
    {
        return nullptr;
    }

    std::vector<MeshVertex> vertices;
    vertices.reserve(segments*segments + 2);

    std::vector<Triangle> indices;
    indices.reserve(segments*segments);


    const float two_pi = glm::two_pi<float>();
    const float pi = glm::pi<float>();
    const float half_pi = glm::half_pi<float>();

    // Our cute insertion vertex
    MeshVertex vertex;

    // See handwritten notes for more detail. But simply speaking, our vertex positions form the direction
    // vector that represent our vertex normal. Simply normalize it by radius since by definition of a sphere,
    // the magnitude = r
    const float inv_r = (1.0f / radius);

    // We start 1 level below the top welding vertex, and end 1 level before the bottom welding vertex
    // These will be added manually to avoid duplicates and post-process the index buffer
    for(int h_segment = 0; h_segment <= segments; h_segment++)
    {
        // Vertical angle off the origin. This decreases along height segments
        const float phi = half_pi - pi*((1.f * h_segment) / segments);
        const float rcos_phi = radius * glm::cos(phi);
        const float rsin_phi = radius * glm::sin(phi);

        for(int w_segment = 0; w_segment <= segments; w_segment++)
        {
            const float theta = two_pi * ((1.f * w_segment) / segments);

            vertex.Position[0] = rcos_phi * glm::cos(theta);   // X-Coordinate
            vertex.Position[1] = rcos_phi * glm::sin(theta);   // Y-Coordinate
            vertex.Position[2] = rsin_phi;                              // Z-Coordinate

            vertex.Normal[0] = vertex.Position[0] / inv_r;
            vertex.Normal[1] = vertex.Position[1] / inv_r;
            vertex.Normal[2] = vertex.Position[2] / inv_r;

            // Spherical UV Mapping
            vertex.UV[0] = (1.f * w_segment) / segments;
            vertex.UV[1] = (1.f * h_segment) / segments;

            vertices.push_back(vertex);
        }
    }

    /*  The following triangulation code was borrowed from: http://www.songho.ca/opengl/gl_sphere.html
    *   I find it to be more succint than what I previously wrote, and handles the top and end caps elegently
    */
    // Need two iterators to conveniently jump from 1 loop to another.
    for(int h_segment = 0; h_segment < segments; h_segment++)
    {
        uint32_t idx1 = h_segment * (segments + 1);
        uint32_t idx2 = idx1 + segments + 1;

        for(int w_segment = 0; w_segment < segments; w_segment++, ++idx1, ++idx2)
        {
            //idx1 = h_segment * (segments+1);
            //idx2 = idx1 + segments + 1;

            if(h_segment != 0)
            {
                //indices.emplace_back(idx1); indices.emplace_back(idx2); indices.emplace_back(idx1 + 1);
                indices.emplace_back(idx1, idx2, idx1 + 1);
            }

            if(h_segment != segments - 1)
            {
                //indices.emplace_back(idx1 + 1); indices.emplace_back(idx2); indices.emplace_back(idx2 + 1);
                indices.emplace_back(idx1 + 1, idx2, idx2 + 1);
            }
        }
    }

    return new StaticMesh(vertices, indices);
}

Mesh* MeshFactory::MakePlane(float width, float length)
{
    std::vector<MeshVertex> vertices;
    const int count = 4;
    vertices.reserve(count);

    // Recall Right Handed convention. X = right, Y = up, Z = back
    MeshVertex v;
    const float x = width / 2.0f;
    const float z = length / 2.0f;

    // Upper Left Corner
    v.Position[0] = -x;
    v.Position[1] = 0;
    v.Position[2] = -z;
    v.Normal[0] = 0.f;
    v.Normal[1] = 1.f;
    v.Normal[2] = 0.f;
    v.UV[0] = 0.f;
    v.UV[1] = 0.f;
    vertices.push_back(v);

    // Lower Left Corner
    v.Position[0] = -x;
    v.Position[1] = 0.f;
    v.Position[2] = z;
    v.Normal[0] = 0.f;
    v.Normal[1] = 1.f;
    v.Normal[2] = 0.f;
    v.UV[0] = 0.f;
    v.UV[1] = 1.f;
    vertices.push_back(v);

    // Upper Right Corner
    v.Position[0] = x;
    v.Position[1] = 0.f;
    v.Position[2] = -z;
    v.Normal[0] = 0.f;
    v.Normal[1] = 1.f;
    v.Normal[2] = 0.f;
    v.UV[0] = 1.f;
    v.UV[1] = 0.f;
    vertices.push_back(v);

    // Lower Right Corner
    v.Position[0] = x;
    v.Position[1] = 0.f;
    v.Position[2] = z;
    v.Normal[0] = 0.f;
    v.Normal[1] = 1.f;
    v.Normal[2] = 0.f;
    v.UV[0] = 1.f;
    v.UV[1] = 1.f;
    vertices.push_back(v);

    std::vector<Triangle> indices;
    indices.reserve(2);

    //indices.emplace_back(0); indices.emplace_back(1); indices.emplace_back(2);
    //indices.emplace_back(1); indices.emplace_back(3); indices.emplace_back(2);
    indices.emplace_back(0, 1, 2);
    indices.emplace_back(1, 3, 2);

    return new StaticMesh(vertices, indices);
}