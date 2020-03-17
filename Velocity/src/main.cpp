#include "vctPCH.h"
#include "Engine/Core/Application.h"
#include "Engine/Core/Log.h"

int main(int argc, char** argv)
{
    Vct::Log::Init();

    VCT_INFO("MAIN!");

    Vct::Application app = Vct::Application();
    app.Run();

    
}