#include "vctPCH.h"
#include "Engine/Core/Application.h"
#include "Engine/Core/Log.h"

#include "Engine/ViewportLayer.h"

#include "Engine/Resources/ResourceMgr.h"
#include "Engine/IO/FileMgr.h"
#include "Engine/IO/FileStream.h"

#include <chrono>
#include <omp.h>

class TextResource : public Resource
{
public:
    TextResource(const std::string& resFile) : Resource(resFile) {}
    TextResource(const Path& resPath) : Resource(resPath) {}

    virtual bool Load(const void* const rawBinary, size_t bytes) override
    {
        return false;
    }
    virtual void Unload() override
    {

    }

protected:
    virtual void OnModified() override {}
    virtual void OnUnloaded() override {}
    virtual bool OnLoaded(bool success, const void* const data, size_t bytes) override {return false;}
    
    void OnLoad(const void* data, size_t bytes)
    {
        int id = omp_get_thread_num();
        VCT_WARN("[{0}]: Text Resource loaded with data address '{1}', number of bytes '{2}'", id, data, bytes);
    }
};

class TextResourceMgr : public TResourceMgr<TextResource, TextResourceMgr>
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

    short     GetExtensionId(const std::string& extension) const override
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

int main(int argc, char** argv)
{
    Vct::Log::Init();

    VCT_INFO("MAIN!");

    FileMgr& mgr = FileMgr::Get();
    mgr.SetBasePath(ASSET_DIR);

    TextResourceMgr& textMgr = TextResourceMgr::Get();
    textMgr.Load(mgr.GetAbsPath("asset1.txt"));
    textMgr.Dump();

    Vct::Application app = Vct::Application();
    app.PushLayer(new Vct::ViewportLayer());
    app.Run();
}