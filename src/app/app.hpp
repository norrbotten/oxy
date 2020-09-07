#pragma once

#include <SFML/Graphics.hpp>

#include "app/imgui_layer.hpp"
#include "app/preview_layer.hpp"

namespace Oxy::Application {

    class App final {
    public:
        App(sf::Vector2u window_size);

        void tick_loop_handler();
        void event_loop_handler(sf::Event& evnt);

        void run();

    private:
        sf::RenderWindow m_window;
        sf::Clock        m_clock;

        ImguiLayer   m_imgui_layer;
        PreviewLayer m_preview_layer;
    };

} // namespace Oxy::Application