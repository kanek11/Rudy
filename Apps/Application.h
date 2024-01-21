#pragma once
#include "RudyPCH.h"



namespace Rudy
{

	//<<interface>>
	class Application
	{
	public:
		virtual ~Application() = default;
		Application() = default;

		virtual void Init() = 0;
		virtual void Start() = 0;




	};


}