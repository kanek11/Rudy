#include "RudyPCH.h"

#include "Application.h"

#include "PBR.h"
#include "NPR.h"
#include "Particles.h"
#include "ClothApp.h"

using namespace Rudy;

Ref<Application> CreateApp(const std::string& appName)
{
    if (appName == "PBR")
    {
        return PBR::Create();
    }
    else if (appName == "NPR")
    {
        return NPR::Create();
    }
    else if (appName == "Particles")
    {
        return Particles::Create();
    }
    else if (appName == "ClothApp")
    {
        return ClothApp::Create();
    }
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

    // auto app = PBR::Create();
    //  auto app = Particles::Create();
    //  auto app = ClothApp::Create();
    //  auto app = NPR::Create();

    std::string appName = "PBR"; // Ä¬ÈÏÓ¦ÓÃ
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg.find("--app=") == 0)
        {
            appName = arg.substr(6);
        }
    }

    auto app = CreateApp(appName);
    if (!app)
    {
        return -1;
    }

    app->Init();
    app->Start();

    return 0;
}