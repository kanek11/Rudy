#include "RudyPCH.h"   
#include <Rudy.h>

  

namespace Rudy
{

	class Pass
	{
	public:
		virtual ~Pass() = default;
		Pass() = default;

	    
		virtual void Render(Ref<Camera>) = 0;

	};








}