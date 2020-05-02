#include "vctPCH.h"
#include "Engine/Core/Application.h"
#include "Engine/Core/Log.h"

#include "Engine/ViewportLayer.h"

#include "Engine/Resources/ResourceMgr.h"
#include "Engine/IO/FileMgr.h"
#include <omp.h>

class TextResource : public Resource
{
public:
    TextResource(const std::string& resFile) : Resource(resFile) {}

    virtual void OnModified() override {}
    virtual void OnUnloaded() override {}
    virtual bool OnLoaded(const void* const data, size_t bytes) override {return false;}
    
    void OnLoad(const void* data, size_t bytes)
    {
        int id = omp_get_thread_num();
        VCT_WARN("[{0}]: Text Resource loaded with data address '{1}', number of bytes '{2}'", id, data, bytes);
    }
};

void OnLoad(const void* const data, size_t bytes)
{
    int id = omp_get_thread_num();
    VCT_WARN("[{0}]: Text Resource loaded with data address '{1}', number of bytes '{2}'", id, data, bytes);
}

void OnAsyncLoad(bool loaded, void* data, size_t bytes)
{
    VCT_WARN("Text Resource loaded with data address '{0}', number of bytes '{1}'", data, bytes);
    free(data);
    data = nullptr;
}

int main(int argc, char** argv)
{
    Vct::Log::Init();

    VCT_INFO("MAIN!");

    /*
    TextResource* asset1 = new TextResource(ResourceMgr::GetResourcePath("asset1.txt"));
    TextResource* asset2 = new TextResource(ResourceMgr::GetResourcePath("asset2.txt"));
    TextResource* asset3 = new TextResource(ResourceMgr::GetResourcePath("asset3.txt"));
    TextResource* asset4 = new TextResource(ResourceMgr::GetResourcePath("asset4.txt"));
    TextResource* asset5 = new TextResource(ResourceMgr::GetResourcePath("asset5.txt"));

    StreamableMgr streamableMgr;
    streamableMgr.EnqueueResource(asset1, BIND_MFN_2(&TextResource::OnLoad, asset1));
    streamableMgr.EnqueueResource(asset2, BIND_MFN_2(&TextResource::OnLoad, asset2));
    streamableMgr.EnqueueResource(asset3, BIND_MFN_2(&TextResource::OnLoad, asset3));
    streamableMgr.EnqueueResource(asset4, BIND_MFN_2(&TextResource::OnLoad, asset4));
    streamableMgr.EnqueueResource(asset5, BIND_MFN_2(&TextResource::OnLoad, asset5));

    streamableMgr.Finish();
    */
    FileMgr& mgr = FileMgr::Get();
    mgr.SetBasePath(ASSET_DIR);

    mgr.LoadAsync("asset1.txt", OnAsyncLoad);
    mgr.LoadAsync("asset2.txt", OnAsyncLoad);
    mgr.LoadAsync("asset3.txt", OnAsyncLoad);
    mgr.LoadAsync("asset4.txt", OnAsyncLoad);
    mgr.LoadAsync("asset5.txt", OnAsyncLoad);
    
    Vct::Application app = Vct::Application();
    app.PushLayer(new Vct::ViewportLayer());
    app.Run();
}