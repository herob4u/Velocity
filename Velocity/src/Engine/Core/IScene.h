#pragma once

class OutputMemoryStream;
class InputMemoryStream;
class World;
namespace Vct
{
    // Interface for a Scene representation. For instance, both the renderer and the physics engine
    // will create and simulate their own scenes that communicate with the game engine.
    struct IScene
    {
        virtual ~IScene() {}

        virtual void Init() = 0;

        virtual World& GetWorld() = 0;
        virtual void Serialize(OutputMemoryStream& os) = 0;
        virtual void Deserialize(InputMemoryStream& is) = 0;

        virtual void Update(float dt) = 0;
        virtual void Pause() = 0;
        virtual void Unpause() = 0;
    };
}