#pragma once

namespace Rudy
{

class GraphicsContext
{
public:
    virtual void Init() = 0;

    static UniquePtr<GraphicsContext> Create(void* window);
};

} // namespace Rudy