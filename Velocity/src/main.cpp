#include "vctPCH.h"
#include "Engine/Core/Application.h"
#include "Engine/Core/Log.h"

#include "Engine/ViewportLayer.h"

#include "Engine/Resources/ResourceMgr.h"
#include "Engine/IO/FileMgr.h"
#include "Engine/IO/FileStream.h"

#include <chrono>
#include <omp.h>

static Resource::Type TextResourceType("text");

class TextResource : public Resource
{
public:
    TextResource(const std::string& resFile) : Resource(resFile) {}
    TextResource(const Path& resPath) : Resource(resPath) {}

    virtual bool Load(const void* const rawBinary, size_t bytes) override
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

class TextResourceMgr : public ResourceMgr
{
public:
    enum TextFormats
    {
        INVALID = 0, TXT = 1, XML, JSON
    };
protected:
    Resource* CreateResource(const Path& resPath) override
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

    void      DestroyResource(Resource& res) override
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
            Resource* res = found->second;
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

#include "Engine/Renderer/Texture/TextureMgr.h"
#include "Engine/Renderer/Texture/Texture.h"


int main(int argc, char** argv)
{
    Vct::Log::Init();

    VCT_INFO("MAIN!");

    FileMgr& mgr = FileMgr::Get();
    mgr.SetBasePath(ASSET_DIR);

    // Register all resource managers by type
    ResourceMgrRegistry::Get().Register(TextResourceType, new TextResourceMgr());
    ResourceMgrRegistry::Get().Register(ResType_Texture, new TextureMgr());

    Vct::Application app = Vct::Application();
    {
        // This should fail!
        TResourcePtr<Texture> testTexture(mgr.GetAbsPath("Pinup_A.tga"));
        VCT_INFO(mgr.GetAbsPath("Pinup_A.tga").GetFullPathRef());
        Texture* texture = testTexture.Load();

        TResourcePtr<TextResource> asset1("asset1.txt");
        asset1.Load();
    }
    app.PushLayer(new Vct::ViewportLayer());
    app.Run();
}