#include "vctPCH.h"
#include "Engine/Core/Application.h"
#include "Engine/Core/Log.h"

#include "Engine/ViewportLayer.h"

#include "Engine/Core/Types/StringId.h"

int main(int argc, char** argv)
{
    Vct::Log::Init();

    VCT_INFO("MAIN!");

    StringId id_none;
    StringId id_none2 = StringId::NONE;
    StringId id_omar = StringId("Omar");
    StringId id_omar2 = StringId("Omar");
    StringId id_omar3 = StringId("omar");

    ASSERT((id_none == StringId::NONE), "TEST 1 FAIL");
    ASSERT((id_omar == id_omar2), "TEST 2 FAIL");
    ASSERT((id_omar != id_omar3), "TEST 3 FAIL");
    ASSERT((id_none2 == id_none), "TEST 4 FAIL");

    VCT_WARN("id_omar: {0}", id_omar.ToString());

    Vct::Application app = Vct::Application();
    app.PushLayer(new Vct::ViewportLayer());
    app.Run();

    
}