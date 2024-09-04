#pragma once
#include "RudyPCH.h"

namespace Rudy
{
// UObject is the base class of all objects
class UObject
{
public:
    virtual ~UObject() = default;
    UObject()          = default;

    std::string Name = "Unnamed Object";
};

// generic factory
template <typename T, typename... Args>
SharedPtr<T> NewObject(const std::string& name = "Unnamed Object", Args&&... args)
{
    auto _obj  = CreateShared<T>(std::forward<Args>(args)...);
    _obj->Name = name;
    return _obj;
}

} // namespace Rudy
