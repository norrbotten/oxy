#include "app/app.hpp"

#include <iostream>

namespace Oxy::Application {

    App::App(sf::Vector2u window_size)
        : m_window(sf::VideoMode(window_size.x, window_size.y), "Oxy")
        , m_preview_layer(m_window) {}

    void App::tick_loop_handler() {
        sf::Event evnt;

        while (m_window.pollEvent(evnt)) {
            ImGui::SFML::ProcessEvent(evnt);

            switch (evnt.type) {
            case sf::Event::Closed: {
                m_window.close();
                break;
            }
            case sf::Event::Resized: {
                m_window.setView(sf::View(sf::FloatRect(0, 0, evnt.size.width, evnt.size.height)));

                ui_event<WindowResized>{}(*this, (int)evnt.size.width, (int)evnt.size.height);
                break;
            }
            default: event_loop_handler(evnt); break;
            }
        }

        ImGui::SFML::Update(m_window, m_clock.restart());

        m_window.clear();

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

            ImGui::Separator();

            if (ImGui::InputText("Render Width", m_window_data.input_render_width, 16)) {
                ui_event<RenderResolutionChanged>{}(*this, m_window_data.input_render_width,
                                                    m_window_data.input_render_height);
            }

            if (ImGui::InputText("Render Height", m_window_data.input_render_height, 16)) {
                ui_event<RenderResolutionChanged>{}(*this, m_window_data.input_render_width,
                                                    m_window_data.input_render_height);
            }

            ImGui::Text("Window Width: %i", m_window_data.window_width);
            ImGui::Text("Window Height: %i", m_window_data.window_height);

            ImGui::Separator();
            ImGui::TreePop();
        }

        ImGui::End();

        m_preview_layer.draw();
        ImGui::SFML::Render(m_window);

        m_window.display();
    }

    void App::event_loop_handler(sf::Event& evnt) {}

    void App::run() {
        ImGui::SFML::Init(m_window);

        ui_event<Initialize>{}(*this);

        while (m_window.isOpen())
            tick_loop_handler();
    }

} // namespace Oxy::Application
