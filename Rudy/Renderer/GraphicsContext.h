#pragma once

namespace Rudy {

	class GraphicsContext
	{
	public:

		virtual void Init() = 0; 

		static Scope<GraphicsContext> Create(void* window);
	};

}