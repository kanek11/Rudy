#include "RudyPCH.h"

#include "Application.h"

#include "deferredPBR.h"
// #include "NPR.h"
// #include "Phys.h"

using namespace Rudy;

ViewportLayer* CreateLayer(const std::string& appName, uint32_t width, uint32_t height)
{
    if (appName == "DeferredPBR")
    {
        return new DeferredPBR(width, height);
    }
    else
    {
        std::cerr << "Unknown app: " << appName << std::endl;
        return nullptr;
    }
}

int main(int argc, char** argv)
{
    //========================================
    // project-global stuff

    Rudy::Log::Init();
    RD_CORE_INFO("test:Initialized Log!");

    // set working directory by filesystem;
    RD_CORE_INFO("Default Current working directory: {0}", std::filesystem::current_path().string());

    auto                  src_path  = std::source_location::current();
    std::filesystem::path this_file = src_path.file_name(); // neccessary conversion

    std::filesystem::current_path(this_file.parent_path().parent_path());
    RD_CORE_INFO("Set Current working directory: {0}", std::filesystem::current_path().string());

    RD_CORE_INFO("size_t on the environment: {0} bytes", sizeof(size_t));

    //
    auto app = CreateShared<Application>();

    std::string appName = "DeferredPBR"; // default app
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg.find("--demo=") == 0)
        {
            appName = arg.substr(6);
        }
    }

    uint32_t APP_WIDTH  = 2560;
    uint32_t APP_HEIGHT = 1600;

    auto viewport_layer = CreateLayer(appName, APP_WIDTH, APP_HEIGHT);
    if (!viewport_layer)
    {
        RD_CORE_ERROR("Failed to create layer: {0}", appName);
        return -1;
    }

    // layer stack
    // app->PushLayer(scene_layer);
    app->m_viewportLayer = viewport_layer;

    // gui
    auto gui_layer    = new ImGuiLayer();
    app->m_imguiLayer = gui_layer;

    app->OnInit();
    app->Run();
    app->OnShutDown();

    return 0;
}