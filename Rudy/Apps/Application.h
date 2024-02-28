#pragma once
#include "RudyPCH.h"
#include "Vendor/imgui/imgui.h"
#include "Vendor/imgui/imgui_impl_glfw.h"
#include "Vendor/imgui/imgui_impl_opengl3.h"

namespace Rudy {

//<<interface>>
class Application {
 public:
  virtual ~Application() = default;
  Application() = default;

  virtual void Init() = 0;
  virtual void Start() = 0;

  virtual void InitGUI() = 0;
  void ShutDownGUI();
  void PrepareGUI();
  virtual void DrawGUI() = 0;
  void RenderGUI();
};

}  // namespace Rudy