#include "vctPCH.h"
#include "Engine.h"

using namespace Vct;

static const std::string s_ShaderCacheFolder = std::string(ROOT_DIR) + "/Cache/";

Vct::VelocityEngine::VelocityEngine()
    : m_ShaderCache(s_ShaderCacheFolder.c_str())
{
}

Vct::VelocityEngine::~VelocityEngine()
{
    m_Renderer.Shutdown();
}

void VelocityEngine::Init()
{
    m_Renderer.Init();
    m_ShaderCache.LoadCache(true);
}

const Renderer& VelocityEngine::GetRenderer() const
{
    return m_Renderer;
}

const ShaderCache& VelocityEngine::GetShaderCache() const
{
    return m_ShaderCache;
}

const ResourceMgrRegistry& VelocityEngine::GetResourceRegistry() const
{
    return ResourceMgrRegistry::Get();
}

Renderer& VelocityEngine::GetRenderer()
{
    return m_Renderer;
}

ShaderCache& VelocityEngine::GetShaderCache()
{
    return m_ShaderCache;
}

ResourceMgrRegistry& VelocityEngine::GetResourceRegistry()
{
    return ResourceMgrRegistry::Get();
}