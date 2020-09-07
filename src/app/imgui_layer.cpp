#include "app/imgui_layer.hpp"

namespace Oxy::Application {

    ImguiLayer::ImguiLayer(sf::RenderWindow& window)
        : m_window(window) {}

    void ImguiLayer::draw() { ImGui::ShowDemoWindow(); }

} // namespace Oxy::Application
