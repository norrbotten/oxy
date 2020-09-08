#include "app/imgui_layer.hpp"

namespace Oxy::Application {

    ImguiLayer::ImguiLayer(sf::RenderWindow& window)
        : m_window(window) {}

    void ImguiLayer::draw() {
        auto HelpMarker = [&](const char* desc) -> void {
            ImGui::TextDisabled("(?)");
            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                ImGui::TextUnformatted(desc);
                ImGui::PopTextWrapPos();
                ImGui::EndTooltip();
            }
        };

        ImGui::Begin("Context Menu");

        if (ImGui::TreeNode("Window")) {
            ImGui::Checkbox("Auto-resize render preview",
                            &m_window_data.auto_resize_render_preview);
            ImGui::SameLine();
            HelpMarker("Resize the render resolution when the window is resized");

            ImGui::Text("Window Width: %i", m_window_params.window_width);
            ImGui::Text("Window Height: %i", m_window_params.window_height);

            ImGui::Text("Render Width: %i", m_window_params.render_width);
            ImGui::SameLine();
            HelpMarker("Width of the rendered image");

            ImGui::Text("Render Height: %i", m_window_params.render_height);
            ImGui::SameLine();
            HelpMarker("Height of the rendered image");

            ImGui::Separator();
            ImGui::TreePop();
        }

        ImGui::End();
    }

} // namespace Oxy::Application
