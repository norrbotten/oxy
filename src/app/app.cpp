#include "app/app.hpp"

#include <iostream>

namespace ImGui {

    bool Button(const char* text, bool disabled) {
        if (disabled) {
            PushStyleVar(ImGuiStyleVar_Alpha, GetStyle().Alpha * 0.5f);
        }

        auto button_res = Button(text);

        if (disabled) {
            PopStyleVar();
        }

        return disabled ? false : button_res;
    }

} // namespace ImGui

namespace Oxy::Application {

    App::App(sf::Vector2u window_size)
        : m_window(sf::VideoMode(window_size.x, window_size.y), "bigbong")
        , m_preview_layer(m_window, 512, 512) {

        resize_render_preview(512, 512);
    }

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

            if (ImGui::BeginCombo("##0.2",
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

            ImGui::SliderFloat("Exposure", &im_render_data.exposure, 0.1, 30, "%f", 1);

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

        ImGui::Begin("Render Control");

        ImGui::Text("Render State: %s", m_renderer.state_str());
        ImGui::Text("Samples Done: %i", m_renderer.samples_done());

        ImGui::Spacing();

        if (ImGui::Button("Start", m_renderer.running())) {
            ui_event<RenderControlStart>{}(*this);
        }

        ImGui::SameLine();

        if (ImGui::Button("Pause", !m_renderer.running())) {
            ui_event<RenderControlPause>{}(*this);
        }

        ImGui::SameLine();

        if (ImGui::Button("Reset", m_renderer.running())) {
            ui_event<RenderControlReset>{}(*this);
        }

        ImGui::End();

        m_preview_layer.draw();
        ImGui::SFML::Render(m_window);

        m_window.display();

        if (!m_renderer.has_block()) {
            m_renderer.next_sample();
            m_renderer.film().copy_to_rgba_buffer(m_preview_layer.get_mutable_buffer(),
                                                  im_render_data.exposure);
        }
    }

    void App::event_loop_handler(sf::Event& evnt) { (void)evnt; }

    void App::run() {
        ImGui::SFML::Init(m_window);

        m_renderer.select_integrator(Renderer::Integrators::Preview);
        resize_render_preview(512, 512);

        m_renderer.camera().set_pos(glm::dvec3(-0.7, -0.2, 0.15));
        m_renderer.camera().aim(glm::dvec3(0, -0.08, 0.05));

        ui_event<Initialize>{}(*this);

        while (m_window.isOpen())
            tick_loop_handler();
    }

} // namespace Oxy::Application
