#include "vctPCH.h"
#include "Engine/Engine.h"
#include "Engine/Core/Application.h"
#include "Engine/Core/Log.h"

#include "Engine/ViewportLayer.h"

#include "Engine/Resources/ResourceMgr.h"
#include "Engine/IO/FileMgr.h"
#include "Engine/IO/FileStream.h"

#include <chrono>
#include <omp.h>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

Vct::Engine* Vct::gEngine = new Vct::VelocityEngine();

static Vct::Resource::Type TextResourceType("text");

class TextResource : public Vct::Resource
{
public:
    TextResource(const std::string& resFile) : Resource(resFile) {}
    TextResource(const Path& resPath) : Resource(resPath) {}

    virtual bool Load(const void* rawBinary, size_t bytes) override
    {
        if(!rawBinary || bytes == 0)
            return false;

        std::string s = std::string((char*)rawBinary, bytes);
        VCT_INFO("Text Loaded: {0}", s);
        VCT_INFO("Loaded: {0}", GetPath());
        return true;
    }

    virtual void Unload() override
    {
        OnUnloaded();
    }

    RES_TYPE(TextResourceType)

protected:    
    void OnLoad(const void* data, size_t bytes)
    {
        int id = omp_get_thread_num();
        VCT_WARN("[{0}]: Text Resource loaded with data address '{1}', number of bytes '{2}'", id, data, bytes);
    }
};

class TextResourceMgr : public Vct::ResourceMgr
{
public:
    enum TextFormats
    {
        INVALID = 0, TXT = 1, XML, JSON
    };
protected:
    Vct::Resource* CreateResource(const Path& resPath) override
    {
        std::string ext(4, '\0');
        resPath.GetExtension(ext);
        TextFormats format = static_cast<TextFormats>(GetExtensionId(ext));

        if(format != TextFormats::INVALID)
        {
            return new TextResource(resPath);
        }

        return nullptr;
    }

    void      DestroyResource(Vct::Resource& res) override
    {
        const Path resPath = res.GetPath();
        if(res.IsLoaded())
        {
            Unload(resPath);
        }

        ASSERT(res.GetRefCount() == 0, "Resource still in use!");
        auto found = m_ResourceList.find(resPath.GetPathId());
        if(found != m_ResourceList.end())
        {
            Vct::Resource* res = found->second;
            if(res)
            {
                delete res;
                res = nullptr;
            }

            m_ResourceList.erase(found);
        }
    }

    short     GetExtensionId(const std::string& extension) const
    {
        if(strcmp(extension.c_str(),"txt") == 0)  
            return TextFormats::TXT;

        else if(strcmp(extension.c_str(), "xml") == 0)
            return TextFormats::XML;

        else if(strcmp(extension.c_str(), "json") == 0) 
            return TextFormats::JSON;

        else 
            return TextFormats::INVALID;
    }
};

#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Texture/Texture2D.h"
#include "Engine/Renderer/Mesh/Model.h"
#include "Engine/Renderer/ShaderCache.h"

int main(int argc, char** argv)
{
    Vct::Log::Init();

    VCT_INFO("MAIN!");
    
    // Initialize the OpenGL context
    Vct::Application app = Vct::Application();

    // Initialize the core engine - this sets up the renderer, resource mgrs, etc for use
    Vct::gEngine->Init();

    FileMgr& mgr = FileMgr::Get();
    mgr.SetBasePath(CONTENT_DIR);

    // Register all resource managers by type
    Vct::ResourceMgrRegistry::Get().Register(TextResourceType, new TextResourceMgr());
    //Vct::Renderer::Get(); // This is placeholder BS. Currently the renderer is responsible for intializing upon construction... this needs to change
    {
        // This should fail!
        /*
        TResourcePtr<Vct::Texture2D> testTexture(mgr.GetAbsPath("Pinup_A.tga"));
        VCT_INFO(mgr.GetAbsPath("Pinup_A.tga").GetFullPathRef());
        Vct::Texture2D* texture = testTexture.Load();
        */

        Vct::TResourcePtr<TextResource> asset1("asset1.txt");
        asset1.Load();
    }
    app.PushLayer(new Vct::ViewportLayer());
    app.Run();
}