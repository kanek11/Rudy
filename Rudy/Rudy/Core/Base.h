#pragma once

#include "Rudy/Core/PlatformDetection.h"

#include <memory>

#ifdef RD_DEBUG
	#if defined(RD_PLATFORM_WINDOWS)
		#define RD_DEBUGBREAK() __debugbreak()
	#elif defined(RD_PLATFORM_LINUX)
		#include <signal.h>
		#define RD_DEBUGBREAK() raise(SIGTRAP)
	#else
		#error "Platform doesn't support debugbreak yet!"
	#endif
	#define RD_ENABLE_ASSERTS
#else
	#define RD_DEBUGBREAK()
#endif

#define RD_EXPAND_MACRO(x) x
#define RD_STRINGIFY_MACRO(x) #x

#define BIT(x) (1 << x)

#define RD_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace Rudy {

	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

}

#include "Rudy/Core/Log.h"
#include "Rudy/Core/Assert.h"
