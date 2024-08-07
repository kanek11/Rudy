#pragma once

#include "Runtime/Core/Base.h"
#include "Runtime/Core/Log.h"

#include "Runtime/Core/PlatformDetection.h"

#ifdef _DEBUG
#    if defined(RD_PLATFORM_WINDOWS)
#        define RD_DEBUGBREAK() __debugbreak()
#    elif defined(RD_PLATFORM_LINUX)
#        include <signal.h>
#        define RD_DEBUGBREAK() raise(SIGTRAP)
#    else
#        error "Platform doesn't support debugbreak yet!"
#    endif
#    define RD_ENABLE_ASSERTS
#else
#    define RD_DEBUGBREAK()
#endif

// me:
#define PATH_SEPARATOR '\\'
#define FILENAME (strrchr(__FILE__, PATH_SEPARATOR) ? strrchr(__FILE__, PATH_SEPARATOR) + 1 : __FILE__)

// IMPL= IMPLEMENTATION
// RD##type##ERROR  will generate like RD_CORE_ERROR or RD_ERROR
// the other branch is with/without message

#ifdef RD_ENABLE_ASSERTS
// provide support for custom formatting by concatenating the formatting string instead of having the format inside the default message
#    define RD_INTERNAL_ASSERT_IMPL(type, check, msg, ...) \
        {                                                  \
            if (!(check))                                  \
            {                                              \
                RD##type##ERROR(msg, __VA_ARGS__);         \
                RD_DEBUGBREAK();                           \
            }                                              \
        }

// branch: with / without message
#    define RD_INTERNAL_ASSERT_WITH_MSG(type, check, ...) RD_INTERNAL_ASSERT_IMPL(type, check, "Assertion condition '{0}' failed at {1}:({2}) :{3}", RD_STRINGIFY_MACRO(check), FILENAME, __LINE__, __VA_ARGS__)
#    define RD_INTERNAL_ASSERT_NO_MSG(type, check) RD_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", RD_STRINGIFY_MACRO(check), FILENAME, __LINE__)

// logic to determine which branch to take by the number of arguments
//  "macro" name
// if #args = 1, then NO_MSG is at macro position;
// if #args = 2, then WITH_MSG is at macro position;
#    define RD_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro_name, ...) macro_name
#    define RD_INTERNAL_ASSERT_GET_MACRO(...) RD_EXPAND_MACRO(RD_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, RD_INTERNAL_ASSERT_WITH_MSG, RD_INTERNAL_ASSERT_NO_MSG))

// branch: CORE / Client , this simply create a new parameter;
#    define RD_ASSERT(...) RD_EXPAND_MACRO(RD_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__))
#    define RD_CORE_ASSERT(...) RD_EXPAND_MACRO(RD_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__))
#else
// do nothing
#    define RD_ASSERT(...)
#    define RD_CORE_ASSERT(...)
#endif
