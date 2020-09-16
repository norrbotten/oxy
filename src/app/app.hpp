#pragma once

#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include <imgui.h>

#include "app/preview_layer.hpp"
#include "renderer/renderer.hpp"

namespace Oxy::Application {

    enum UIEvent {
        Initialize,

        WindowResized,
        RenderResolutionChanged,

        RenderPreviewEnabledToggled,
        RenderPreviewModeChanged,
        RenderAlgorithmChanged,
        RenderMaxSamplesChanged,
        RenderContinousSamplingToggled,

        RaytracerSupersamplingChanged,

        PathtracerMethodChanged,

        RenderControlStart,
        RenderControlPause,
        RenderControlReset,
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

    struct ImmediateData_RenderSettings {
        bool preview = false;

        const char* preview_modes[4] = {"Albedo", "Normal", "Flat", "Triangle"};
        const char* preview_help[4]  = {"Show object colors only", "Show world space normals",
                                       "Show with flat shading", "Colorize triangles randomly"};
        int         preview_mode     = 0;

        const char* rendering_modes[2]      = {"Raytracing", "Pathtracing"};
        const char* rendering_modes_help[2] = {"Simple raytracing, no indirect lighting",
                                               "Full pathtracing, global illumination"};
        int         rendering_mode          = 0;

        bool continous_sampling = true;
        int  max_samples        = 32;

        float exposure = 1;
    };

    struct ImmediateData_RaytracingSettings {
        const char* supersampling_modes[4]      = {"1x", "2x", "4x", "8x"};
        const char* supersampling_modes_help[4] = {"Same as default", "Sample 4 times per pixel",
                                                   "Sample 16 times per pixel",
                                                   "Sample 64 times per pixel"};
        int         supersampling_mode          = 1;
    };

    struct ImmediateData_PathtracerSettings {
        const char* pathtracer_integrators[5] = {"Naive", "Naive-Direct sampling", "BDPT", "MLT",
                                                 "MLT+BDPT"};
        const char* pathtracer_integrators_help[5] = {
            "Simplest possible algorithm, very slow and noisy",
            "Same as naive, but sample lights implicitly",
            "Bidirectional path tracing, better at rendering indirect light and caustics",
            "Metropolis light transport", "Metropolis light transport combined with BDPT"};
        int pathtracer_integrator = 0;
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
            m_renderer.set_render_resolution(width, height);
        }

        auto& renderer() { return m_renderer; }

        ImmediateData_Window             im_window_data;
        ImmediateData_RenderSettings     im_render_data;
        ImmediateData_RaytracingSettings im_rt_data;
        ImmediateData_PathtracerSettings im_pt_data;

    private:
        sf::RenderWindow m_window;
        sf::Clock        m_clock;

        PreviewLayer m_preview_layer;

        Renderer::OxyRenderer m_renderer;
    };

    template <UIEvent evnt, typename... Args>
    struct ui_event {
        void operator()(App& app, Args... args);
    };

    template <>
    struct ui_event<WindowResized> {
        void operator()(App& app, int width, int height) {
            app.im_window_data.window_width  = width;
            app.im_window_data.window_height = height;

            if (app.im_window_data.auto_resize_render_preview) {
                app.im_window_data.render_width  = width;
                app.im_window_data.render_height = height;

                snprintf(app.im_window_data.input_render_width, 16, "%i", width);
                snprintf(app.im_window_data.input_render_height, 16, "%i", height);

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

                app.im_window_data.render_width  = width_val;
                app.im_window_data.render_height = height_val;

                snprintf(app.im_window_data.input_render_width, 16, "%i", width_val);
                snprintf(app.im_window_data.input_render_height, 16, "%i", height_val);
            }
            catch (...) {
                width_val  = app.im_window_data.render_width;
                height_val = app.im_window_data.render_height;
            }

            app.resize_render_preview(width_val, height_val);
        }
    };

    template <>
    struct ui_event<RenderPreviewEnabledToggled> {
        void operator()(App& app, bool on) {
            (void)app;
            (void)on;
        }
    };

    template <>
    struct ui_event<RenderPreviewModeChanged> {
        void operator()(App& app, int mode) {
            (void)app;
            (void)mode;
        }
    };

    template <>
    struct ui_event<RenderAlgorithmChanged> {
        void operator()(App& app, int mode) {
            (void)app;
            (void)mode;
        }
    };

    template <>
    struct ui_event<RenderMaxSamplesChanged> {
        void operator()(App& app, int num_samples) { app.renderer().set_max_samples(num_samples); }
    };

    template <>
    struct ui_event<RenderContinousSamplingToggled> {
        void operator()(App& app, bool on) { app.renderer().sample_continously(on); }
    };

    template <>
    struct ui_event<RaytracerSupersamplingChanged> {
        void operator()(App& app, int level) {
            (void)app;
            (void)level;
        }
    };

    template <>
    struct ui_event<PathtracerMethodChanged> {
        void operator()(App& app, int method) {
            (void)app;
            (void)method;
        }
    };

    template <>
    struct ui_event<RenderControlStart> {
        void operator()(App& app) { app.renderer().start_render(); }
    };

    template <>
    struct ui_event<RenderControlPause> {
        void operator()(App& app) { app.renderer().pause_render(); }
    };

    template <>
    struct ui_event<RenderControlReset> {
        void operator()(App& app) { app.renderer().reset_render(); }
    };

    template <>
    struct ui_event<Initialize> {
        void operator()(App& app) {
            app.im_window_data.window_width  = app.window_size().x;
            app.im_window_data.window_height = app.window_size().y;

            app.im_window_data.render_width  = app.render_preview_resolution().x;
            app.im_window_data.render_height = app.render_preview_resolution().y;

            snprintf(app.im_window_data.input_render_width, 16, "%i",
                     app.im_window_data.render_width);
            snprintf(app.im_window_data.input_render_height, 16, "%i",
                     app.im_window_data.render_height);

            ui_event<RenderMaxSamplesChanged>{}(app, app.im_render_data.max_samples);
            ui_event<RenderContinousSamplingToggled>{}(app, app.im_render_data.continous_sampling);
        }
    };

} // namespace Oxy::Application