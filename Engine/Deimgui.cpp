#include "Deimgui.h"
#include <cstdio>

// ================================================================
//  Lifecycle
// ================================================================

void DebugUI::Init(GLFWwindow* window, const char* glslVersion)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glslVersion);
}

void DebugUI::Shutdown()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void DebugUI::BeginFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void DebugUI::EndFrame()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

// ================================================================
//  Panels
// ================================================================

void DebugUI::RegisterPanel(const std::string& name, std::function<void()> drawFn)
{
    UIPanel p;
    p.name = name;
    p.draw = drawFn;
    m_panels.push_back(p);
}

void DebugUI::RenderPanels()
{
    for (auto& panel : m_panels)
    {
        if (!panel.open) continue;
        ImGui::Begin(panel.name.c_str(), &panel.open);
        panel.draw();          // run the lambda
        ImGui::End();
    }
}

// ================================================================
//  Built-in helpers
// ================================================================

void DebugUI::ColourPicker3(const char* label, glm::vec3& colour)
{
    // ImGui works with float[3] so we pass the address of the first component
    ImGui::ColorEdit3(label, &colour.r);
}

void DebugUI::ColourPicker4(const char* label, glm::vec4& colour)
{
    ImGui::ColorEdit4(label, &colour.r);
}

void DebugUI::TexturePreview(const char* label,
                              unsigned int texID,
                              float width, float height)
{
    ImGui::Text("%s", label);
    // ImTextureID is just a void* on most backends
    ImGui::Image((ImTextureID)(uintptr_t)texID,
                 ImVec2(width, height),
                 ImVec2(0, 1),   // UV flip so texture is right-side-up
                 ImVec2(1, 0));
}

void DebugUI::TransformWidget(const char* label,
                               glm::vec3&  position
                               //glm::vec3&  rotation,
                               //float&      scale
                               )
{
    ImGui::PushID(label);
    ImGui::Text("%s", label);
    ImGui::DragFloat3("Position", &position.x, 0.01f);
  //  ImGui::DragFloat3("Rotation", &rotation.x, 0.5f, -180.f, 180.f);
   // ImGui::DragFloat ("Scale",    &scale,       0.01f, 0.01f, 10.f);
    ImGui::PopID();
}

// ================================================================
//  Log
// ================================================================

void DebugUI::Log(const std::string& msg)
{
    // Keep at most 200 lines so it never eats memory
    if (m_log.size() > 200)
        m_log.erase(m_log.begin());
    m_log.push_back(msg);
}

void DebugUI::RenderLog(const char* panelName)
{
    ImGui::Begin(panelName);
    if (ImGui::Button("Clear")) m_log.clear();
    ImGui::Separator();
    ImGui::BeginChild("scrollregion");
    for (const auto& line : m_log)
        ImGui::TextUnformatted(line.c_str());
    // Auto-scroll to bottom
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);
    ImGui::EndChild();
    ImGui::End();
}

// ================================================================
//  FPS overlay
// ================================================================

void DebugUI::FPSOverlay()
{
    // Transparent overlay pinned to top-right corner
    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoDecoration      |
        ImGuiWindowFlags_AlwaysAutoResize  |
        ImGuiWindowFlags_NoSavedSettings   |
        ImGuiWindowFlags_NoFocusOnAppearing|
        ImGuiWindowFlags_NoNav             |
        ImGuiWindowFlags_NoMove;

    ImGui::SetNextWindowBgAlpha(0.35f);
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 10.f, 10.f),
                            ImGuiCond_Always, ImVec2(1.f, 0.f));
    ImGui::Begin("FPS", nullptr, flags);
    ImGui::Text("%.1f FPS  (%.2f ms)", ImGui::GetIO().Framerate,
                1000.f / ImGui::GetIO().Framerate);
    ImGui::End();
}

// ================================================================
//  Intro screen
// ================================================================

bool DebugUI::IntroScreen(const char* message)
{
    if (!m_showIntro) return false;

    // Full-screen dark overlay
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowSize(io.DisplaySize);
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowBgAlpha(0.75f);

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoMove       |
        ImGuiWindowFlags_NoSavedSettings;

    ImGui::Begin("Intro", nullptr, flags);

    // Centre the text
    ImVec2 textSize = ImGui::CalcTextSize(message);
    ImGui::SetCursorPos(ImVec2(
        (io.DisplaySize.x - textSize.x) * 0.5f,
        (io.DisplaySize.y - textSize.y) * 0.5f
    ));
    ImGui::Text("%s", message);
    ImGui::End();

    // Dismiss on SPACE via ImGui input (no GLFW needed here)
    if (ImGui::IsKeyPressed(ImGuiKey_Space))
        m_showIntro = false;

    return m_showIntro;
}
