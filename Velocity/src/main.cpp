#include "vctPCH.h"
#include "Engine/Core/Application.h"
#include "Engine/Core/Log.h"

#include "Engine/ViewportLayer.h"

#include "Engine/Resources/ResourceMgr.h"
#include "Engine/IO/FileMgr.h"
#include "Engine/IO/FileStream.h"
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

    FileMgr& mgr = FileMgr::Get();
    mgr.SetBasePath(ASSET_DIR);

    mgr.LoadAsync("asset1.txt", OnAsyncLoad);
    mgr.LoadAsync("asset2.txt", OnAsyncLoad);
    mgr.LoadAsync("asset3.txt", OnAsyncLoad);
    mgr.LoadAsync("asset4.txt", OnAsyncLoad);
    mgr.LoadAsync("asset5.txt", OnAsyncLoad);
    
    InputFileStream ifile;
    ifile.Open((std::string(ASSET_DIR) + "asset1.txt").c_str());

    char str[13] = "aaaaaaaaaaaa";
    VCT_INFO("Curr String: {0}", str);

    ifile >> str;

    VCT_INFO("Got String: {0}", str);

    char c;
    while(ifile)
    {
        ifile >> c;
        VCT_INFO("{0}", c);
    }

    Vct::Application app = Vct::Application();
    app.PushLayer(new Vct::ViewportLayer());
    app.Run();
}