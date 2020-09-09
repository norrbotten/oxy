#include "app/app.hpp"

#include <iostream>

namespace Oxy::Application {

    App::App(sf::Vector2u window_size)
        : m_window(sf::VideoMode(window_size.x, window_size.y), "bigbong")
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
                            &im_window_data.auto_resize_render_preview);
            ImGui::SameLine();
            HelpMarker("Resize the render resolution when the window is resized");

            ImGui::Spacing();

            ImGui::Text("Render Width");
            if (ImGui::InputText("##a", im_window_data.input_render_width, 16)) {
                ui_event<RenderResolutionChanged>{}(*this, im_window_data.input_render_width,
                                                    im_window_data.input_render_height);
            }

            ImGui::Text("Render Height");
            if (ImGui::InputText("##b", im_window_data.input_render_height, 16)) {
                ui_event<RenderResolutionChanged>{}(*this, im_window_data.input_render_width,
                                                    im_window_data.input_render_height);
            }

            ImGui::Spacing();

            ImGui::Text("Window Width: %i", im_window_data.window_width);
            ImGui::Text("Window Height: %i", im_window_data.window_height);

            ImGui::Spacing();
            ImGui::Spacing();

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Render Settings")) {
            if (ImGui::Checkbox("Enable Preview", &im_render_data.preview)) {
                ui_event<RenderPreviewEnabledToggled>{}(*this, im_render_data.preview);
            }

            ImGui::SameLine();
            HelpMarker("A simpler rendering mode for fast preview");

            ImGui::Spacing();

            ImGui::Text("Preview Mode");

            if (ImGui::BeginCombo("##1",
                                  im_render_data.preview_modes[im_render_data.preview_mode])) {

                for (int i = 0; i < IM_ARRAYSIZE(im_render_data.preview_modes); i++) {
                    auto selected = (i == im_render_data.preview_mode);
                    auto label    = im_render_data.preview_modes[i];

                    if (ImGui::Selectable(label, selected)) {
                        im_render_data.preview_mode = i;
                        selected                    = true;

                        ui_event<RenderPreviewModeChanged>{}(*this, i);
                    }

                    ImGui::SameLine();
                    HelpMarker(im_render_data.preview_help[i]);

                    if (selected)
                        ImGui::SetItemDefaultFocus();
                }

                ImGui::EndCombo();
            }

            ImGui::Spacing();

            ImGui::Text("Rendering Algorithm");

            if (ImGui::BeginCombo("##2",
                                  im_render_data.rendering_modes[im_render_data.rendering_mode])) {

                for (int i = 0; i < IM_ARRAYSIZE(im_render_data.rendering_modes); i++) {
                    auto selected = (i == im_render_data.rendering_mode);
                    auto label    = im_render_data.rendering_modes[i];

                    if (ImGui::Selectable(label, selected)) {
                        im_render_data.rendering_mode = i;
                        selected                      = true;

                        ui_event<RenderAlgorithmChanged>{}(*this, i);
                    }

                    ImGui::SameLine();
                    HelpMarker(im_render_data.rendering_modes_help[i]);

                    if (selected)
                        ImGui::SetItemDefaultFocus();
                }

                ImGui::EndCombo();
            }

            ImGui::Spacing();

            if (ImGui::InputInt("Max samples", &im_render_data.max_samples, 1, 100)) {
                if (im_render_data.max_samples < 1) {
                    im_render_data.max_samples = 1;
                }

                if (im_render_data.max_samples > 1e9) {
                    im_render_data.max_samples = 1e9;
                }

                ui_event<RenderMaxSamplesChanged>{}(*this, im_render_data.max_samples);
            }

            ImGui::Spacing();

            if (ImGui::Checkbox("Continous sampling", &im_render_data.continous_sampling)) {
                ui_event<RenderContinousSamplingToggled>{}(*this,
                                                           im_render_data.continous_sampling);
            }
            ImGui::SameLine();
            HelpMarker("Don't stop at max samples");

            ImGui::Spacing();
            ImGui::Spacing();

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Raytracer Settings")) {
            ImGui::Text("Supersampling mode");

            if (ImGui::BeginCombo("##3",
                                  im_rt_data.supersampling_modes[im_rt_data.supersampling_mode])) {

                for (int i = 0; i < IM_ARRAYSIZE(im_rt_data.supersampling_modes); i++) {
                    auto selected = (i == im_rt_data.supersampling_mode);
                    auto label    = im_rt_data.supersampling_modes[i];

                    if (ImGui::Selectable(label, selected)) {
                        im_rt_data.supersampling_mode = i;
                        selected                      = true;

                        ui_event<RaytracerSupersamplingChanged>{}(*this, i);
                    }

                    ImGui::SameLine();
                    HelpMarker(im_rt_data.supersampling_modes_help[i]);

                    if (selected)
                        ImGui::SetItemDefaultFocus();
                }

                ImGui::EndCombo();
            }

            ImGui::Spacing();
            ImGui::Spacing();

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Pathtracer Settings")) {
            ImGui::Text("Pathtracing Method");

            if (ImGui::BeginCombo(
                    "##4", im_pt_data.pathtracer_integrators[im_pt_data.pathtracer_integrator])) {

                for (int i = 0; i < IM_ARRAYSIZE(im_pt_data.pathtracer_integrators); i++) {
                    auto selected = (i == im_pt_data.pathtracer_integrator);
                    auto label    = im_pt_data.pathtracer_integrators[i];

                    if (ImGui::Selectable(label, selected)) {
                        im_pt_data.pathtracer_integrator = i;
                        selected                         = true;

                        ui_event<PathtracerMethodChanged>{}(*this, i);
                    }

                    ImGui::SameLine();
                    HelpMarker(im_pt_data.pathtracer_integrators_help[i]);

                    if (selected)
                        ImGui::SetItemDefaultFocus();
                }

                ImGui::EndCombo();
            }

            ImGui::Spacing();
            ImGui::Spacing();

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
