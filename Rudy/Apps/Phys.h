#include "RudyPCH.h"

#include "Vendor/imgui/imgui.h"
#include "Vendor/imgui/imgui_impl_glfw.h"
#include "Vendor/imgui/imgui_impl_opengl3.h"

#include "Rudy/Particles/ParticleSystem.h"
#include "Rudy/Physics/Cloth.h"
#include "Rudy/Physics/Fluid.h"

#include "Application.h"

namespace Rudy
{

class Phys : public ViewportLayer
{
public:
    ~Phys() = default;
    Phys();

    void Init() override;
    void ShutDown() override;

    virtual void OnUpdate(float ts) override;
    virtual void OnImGuiRender() override;

    // app specific=======================

    // particles
    SharedPtr<Emitter>          particle = nullptr;
    SharedPtr<StaticMeshObject> floor    = nullptr;

    // cloth
    Cloth* cloth = nullptr;

    // fluid
    Fluid* fluid = nullptr;

    bool enableCloth     = false;
    bool enableParticles = false;
    bool enableFluid     = true;
};

} // namespace Rudy
