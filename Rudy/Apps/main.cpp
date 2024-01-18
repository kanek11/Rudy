#include "RudyPCH.h"

#include "Application.h"

#include "PBR.h"

using namespace Rudy;
int main()
{

	auto app = PBR::Create();
	app->Init();
	app->Start();

 

	return 0;
}