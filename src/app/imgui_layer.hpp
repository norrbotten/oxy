#pragma once

#include <imgui-SFML.h>
#include <imgui.h>

namespace Oxy::Application {

    struct ImmediateData_Window {
        bool auto_resize_render_preview = false;
    };

    struct ImmediateParams_Window {
        int window_width;
        int window_height;

        int render_width;
        int render_height;
    };

    class ImguiLayer final {
    public:
        ImguiLayer(sf::RenderWindow& window);

        void draw();

        const auto& data_window() { return m_window_data; }
        auto&       params_window() { return m_window_params; }

    private:
        sf::RenderWindow& m_window;

        ImmediateData_Window   m_window_data;
        ImmediateParams_Window m_window_params;
    };

} // namespace Oxy::Application