#pragma once

#include <imgui-SFML.h>
#include <imgui.h>

namespace Oxy::Application {

    class ImguiLayer final {
    public:
        ImguiLayer(sf::RenderWindow& window);

        void draw();

    private:
        sf::RenderWindow& m_window;
    };

} // namespace Oxy