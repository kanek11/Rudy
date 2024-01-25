#include "RudyPCH.h"
#include "Application.h"


namespace Rudy
{
	 



     
    void Application::ShutDownGUI()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }


    void Application::PrepareGUI()
    {
        // ��ʼ�µ�һ֡
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //  
        this->DrawGUI();
    } 


    void Application::RenderGUI()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

	
}