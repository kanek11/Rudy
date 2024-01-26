#pragma once

#include "Rudy/Core/Base.h"
#include "Rudy/Core/Log.h" 

#ifdef RD_ENABLE_ASSERTS

	// Alteratively we could use the same "default" message for both "WITH_MSG" and "NO_MSG" and
	// provide support for custom formatting by concatenating the formatting string instead of having the format inside the default message
	#define RD_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { RD##type##ERROR(msg, __VA_ARGS__); RD_DEBUGBREAK(); } }
	#define RD_INTERNAL_ASSERT_WITH_MSG(type, check, ...) RD_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
	#define RD_INTERNAL_ASSERT_NO_MSG(type, check) RD_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", RD_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

	#define RD_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
	#define RD_INTERNAL_ASSERT_GET_MACRO(...) RD_EXPAND_MACRO( RD_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, RD_INTERNAL_ASSERT_WITH_MSG, RD_INTERNAL_ASSERT_NO_MSG) )

	// Currently accepts at least the condition and one additional parameter (the message) being optional
	#define RD_ASSERT(...) RD_EXPAND_MACRO( RD_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
	#define RD_CORE_ASSERT(...) RD_EXPAND_MACRO( RD_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )
#else
	#define RD_ASSERT(...)
	#define RD_CORE_ASSERT(...)
#endif
