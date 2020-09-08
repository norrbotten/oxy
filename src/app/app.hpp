#pragma once

#include <SFML/Graphics.hpp>

#include "app/preview_layer.hpp"

#include <imgui.h>

#include <imgui-SFML.h>

namespace Oxy::Application {

    enum UIEvent {
        Initialize,
        WindowResized,
        RenderResolutionChanged,
    };

    struct ImmediateData_Window {
        bool auto_resize_render_preview = false;

        int window_width;
        int window_height;

        int render_width;
        int render_height;

        char input_render_width[16];
        char input_render_height[16];
    };

    class App final {
    public:
        App(sf::Vector2u window_size);

        sf::Vector2u window_size() const { return m_window.getSize(); }
        sf::Vector2u render_preview_resolution() const { return m_preview_layer.resolution(); }

        void tick_loop_handler();
        void event_loop_handler(sf::Event& evnt);

        void run();

        void resize_render_preview(int width, int height) {
            m_preview_layer.resize(sf::Vector2u(width, height));
        }

        ImmediateData_Window m_window_data;

    private:
        sf::RenderWindow m_window;
        sf::Clock        m_clock;

        PreviewLayer m_preview_layer;
    };

    template <UIEvent evnt, typename... Args>
    struct ui_event {
        void operator()(App& app, Args... args);
    };

    template <>
    struct ui_event<Initialize> {
        void operator()(App& app) {
            app.m_window_data.window_width  = app.window_size().x;
            app.m_window_data.window_height = app.window_size().y;

            app.m_window_data.render_width  = app.render_preview_resolution().x;
            app.m_window_data.render_height = app.render_preview_resolution().y;

            snprintf(app.m_window_data.input_render_width, 16, "%i",
                     app.m_window_data.render_width);
            snprintf(app.m_window_data.input_render_height, 16, "%i",
                     app.m_window_data.render_height);
        }
    };

    template <>
    struct ui_event<WindowResized> {
        void operator()(App& app, int width, int height) {
            app.m_window_data.window_width  = width;
            app.m_window_data.window_height = height;

            if (app.m_window_data.auto_resize_render_preview) {
                app.m_window_data.render_width  = width;
                app.m_window_data.render_height = height;

                snprintf(app.m_window_data.input_render_width, 16, "%i", width);
                snprintf(app.m_window_data.input_render_height, 16, "%i", height);

                app.resize_render_preview(width, height);
            }
        }
    };

    template <>
    struct ui_event<RenderResolutionChanged> {
        void operator()(App& app, const char* width_str, const char* height_str) {
            int width_val, height_val;

            try {
                width_val  = std::stoi(width_str);
                height_val = std::stoi(height_str);

                if (width_val > 16384)
                    width_val = 16384;

                if (width_val < 1)
                    width_val = 1;

                if (height_val > 16384)
                    height_val = 16384;

                if (height_val < 1)
                    height_val = 1;

                app.m_window_data.render_width  = width_val;
                app.m_window_data.render_height = height_val;

                snprintf(app.m_window_data.input_render_width, 16, "%i", width_val);
                snprintf(app.m_window_data.input_render_height, 16, "%i", height_val);
            }
            catch (...) {
                width_val  = app.m_window_data.render_width;
                height_val = app.m_window_data.render_height;
            }

            app.resize_render_preview(width_val, height_val);
        }
    };

} // namespace Oxy::Application