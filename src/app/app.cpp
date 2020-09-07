#include "app/app.hpp"

namespace Oxy::Application {

    App::App(sf::Vector2u window_size)
        : m_window(sf::VideoMode(window_size.x, window_size.y), "Oxy")
        , m_imgui_layer(m_window)
        , m_preview_layer(m_window) {}

    void App::tick_loop_handler() {
        sf::Event evnt;

        while (m_window.pollEvent(evnt)) {
            ImGui::SFML::ProcessEvent(evnt);

            switch (evnt.type) {
            case sf::Event::Closed: m_window.close(); break;
            case sf::Event::Resized:
                m_window.setView(sf::View(sf::FloatRect(0, 0, evnt.size.width, evnt.size.height)));
                break;
            default: event_loop_handler(evnt); break;
            }
        }

        ImGui::SFML::Update(m_window, m_clock.restart());

        m_window.clear();

        m_preview_layer.draw();

        m_imgui_layer.draw();
        ImGui::SFML::Render(m_window);

        m_window.display();
    }

    void App::event_loop_handler(sf::Event& evnt) {}

    void App::run() {
        ImGui::SFML::Init(m_window);

        while (m_window.isOpen())
            tick_loop_handler();
    }

} // namespace Oxy::Application
