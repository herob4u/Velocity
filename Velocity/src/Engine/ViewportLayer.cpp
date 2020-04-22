#include "vctPCH.h"
#include "ViewportLayer.h"

using namespace Vct;

ViewportLayer::ViewportLayer()
    : Layer("ViewportLayer")
{
}

void ViewportLayer::OnAttached()
{
}

void ViewportLayer::OnUpdate(float dt)
{
    RenderCommands::ClearBuffers();
    RenderCommands::SetClearColor(0.2f, 0.2f, 0.2f);
    //VCT_TRACE("Asset Dir: {0}", ASSET_DIR);
}

void ViewportLayer::OnDetached()
{
}
