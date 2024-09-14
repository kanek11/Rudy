#pragma once
#include <memory>

// basic macros;

// see assert
#define RD_EXPAND_MACRO(x) x
#define RD_STRINGIFY_MACRO(x) #x

namespace Rudy
{

// sole owner
template <typename T>
using UniquePtr = std::unique_ptr<T>;
template <typename T, typename... Args>
constexpr UniquePtr<T> CreateUnique(Args&&... args)
{
    return std::make_unique<T>(std::forward<Args>(args)...);
}

// shared owner
template <typename T>
using SharedPtr = std::shared_ptr<T>;
template <typename T, typename... Args>
constexpr SharedPtr<T> CreateShared(Args&&... args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}

// weak reference no ownership
template <typename T>
using WeakPtr = std::weak_ptr<T>;
template <typename T, typename... Args>
constexpr WeakPtr<T> CreateWeak(std::shared_ptr<T> sharedPtr)
{
    return WeakPtr<T>(sharedPtr);
}
} // namespace Rudy
