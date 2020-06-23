#pragma once

#include "Renderer/Renderer.h"
#include "Renderer/ShaderCache.h"
#include "Resources/ResourceMgr.h"

namespace Vct
{
    class Engine
    {
    public:
        virtual ~Engine() {}

        virtual void Init() = 0;

        virtual const Renderer& GetRenderer() const = 0;
        virtual const ShaderCache& GetShaderCache() const = 0;
        virtual const ResourceMgrRegistry& GetResourceRegistry() const = 0;

        virtual Renderer& GetRenderer() = 0;
        virtual ShaderCache& GetShaderCache() = 0;
        virtual ResourceMgrRegistry& GetResourceRegistry() = 0;

        virtual void HandleEvents() = 0;
    protected:
        
    private:

    };

    class VelocityEngine final : public Engine
    {
    public:
        VelocityEngine();
        ~VelocityEngine();

        virtual void Init() override;

        virtual const Renderer& GetRenderer() const override;
        virtual const ShaderCache& GetShaderCache() const override;
        virtual const ResourceMgrRegistry& GetResourceRegistry() const override;

        virtual Renderer& GetRenderer() override;
        virtual ShaderCache& GetShaderCache() override;
        virtual ResourceMgrRegistry& GetResourceRegistry() override;

        virtual void HandleEvents() override;
    protected:
    private:
        Renderer m_Renderer;
        ShaderCache m_ShaderCache;
    };
}