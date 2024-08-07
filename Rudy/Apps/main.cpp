#include "RudyPCH.h"

#include "Application.h"

#include "PBR.h"
// #include "NPR.h"
// #include "Phys.h"

using namespace Rudy;

ViewportLayer* CreateLayer(const std::string& appName)
{
    if (appName == "PBR")
    {
        return new PBR();
    }
    // else if (appName == "NPR")
    // {
    //     return new NPR();
    // }
    // else if (appName == "Phys")
    // {
    //     return new Phys();
    // }
    else
    {
        std::cerr << "Unknown app: " << appName << std::endl;
        return nullptr;
    }
}

int main(int argc, char** argv)
{
    // Rudy::Log::Init();
    // RD_CORE_WARN("test:Initialized Log!");
    // RD_ASSERT(2 < 1, "test:Assert!");

    auto app = CreateShared<Application>();

    std::string appName = "PBR"; // default app
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg.find("--demo=") == 0)
        {
            appName = arg.substr(6);
        }
    }

    auto viewport_layer = CreateLayer(appName);
    if (!viewport_layer)
    {
        RD_CORE_ERROR("Failed to create layer: {0}", appName);
        return -1;
    }

    // layer stack
    // app->PushLayer(scene_layer);
    app->m_viewportLayer = viewport_layer;

    // PBR 4K
    viewport_layer->SCR_WIDTH  = 2560;
    viewport_layer->SCR_HEIGHT = 1440;

    // phys
    //  viewport_layer->SCR_HEIGHT = 1080;
    //  viewport_layer->SCR_WIDTH  = 1920;

    // gui
    auto gui_layer    = new ImGuiLayer();
    app->m_imguiLayer = gui_layer;

    app->Init();
    app->Run();
    app->ShutDown();

    return 0;
}