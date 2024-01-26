#include "RudyPCH.h"

#include "Application.h"

#include "PBR.h"
#include "NPR.h"
#include "Particles.h"
#include "ClothApp.h"

using namespace Rudy;
int main()
{
	//Rudy::Log::Init();
	//RD_CORE_WARN("test:Initialized Log!");
	//RD_ASSERT(2 < 1, "test:Assert!");



    //auto app = PBR::Create();
	 //auto app = Particles::Create();
	// auto app = ClothApp::Create();

    auto app = NPR::Create();
	app->Init();
	app->Start();  


	return 0;
}