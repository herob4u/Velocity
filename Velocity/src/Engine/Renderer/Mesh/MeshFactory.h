#pragma once
namespace Vct
{
    class Mesh;

    class MeshFactory
    {
    public:
        // We really seek a faceted look on cubes by splitting their normals, but having special
        // cases where vertices have 2 normals versus 1 makes for an overcomplication I do not want to tackle
        static Mesh* MakeBox(float length, float width, float height);

        static Mesh* MakeSphere(float radius, int segments);

        static Mesh* MakePlane(float width, float length);
    };
}