#include "RudyPCH.h"

#include "Application.h"

#include "PBR.h"
#include "NPR.h"

using namespace Rudy;
int main()
{

	//auto app = PBR::Create();
	auto app = NPR::Create();
	app->Init();
	app->Start(); 

	return 0;
}