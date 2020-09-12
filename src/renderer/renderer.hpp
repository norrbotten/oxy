#pragma once

#include <iostream>
#include <mutex>
#include <optional>
#include <thread>
#include <vector>

#include "renderer/integrators/integrator.hpp"
#include "renderer/integrators/preview_integrator.hpp"

#include "renderer/utils/sample_film.hpp"

namespace Oxy::Renderer {

    struct Block {
        int start_x, start_y;
        int end_x, end_y;
    };

    enum class WorkerState {
        Rendering,
        Paused,
        Stopped,
    };

    class OxyRenderer final {
    public:
        OxyRenderer();
        ~OxyRenderer();

        auto get_render_width() const { return m_film.width(); }
        auto get_render_height() const { return m_film.height(); }

        void set_render_resolution(int width, int height) {
            if (m_integrator != nullptr)
                m_integrator->set_resolution(width, height);

            m_film.resize(width, height);
        }

        void select_integrator(Integrators integrator) {
            if (m_integrator != nullptr)
                delete m_integrator;

            switch (integrator) {
            case Integrators::Fractal:
                m_integrator = new FractalIntegrator(m_film.width(), m_film.height(), m_film);
                break;

            case Integrators::Buddhabrot:
                m_integrator = new BuddhabrotIntegrator(m_film.width(), m_film.height(), m_film);
                break;

            case Integrators::Preview:
                m_integrator = new PreviewIntegrator(m_film.width(), m_film.height(), m_film);
                break;
            }

            std::vector<Triangle> triangles;

            triangles.push_back(
                Triangle(glm::dvec3(0, -1, -1), glm::dvec3(0, 1, -1), glm::dvec3(0, 0, 1)));

            m_integrator->accel().triangle_bvh().build(triangles);

            m_film.clear();
        }

        void start_render(unsigned int num_threads = 24);
        void pause_render();
        void reset_render();

        const auto& film() const { return m_film; }
        const auto& blocks() const { return m_blocks; }

        auto& camera() { return m_camera; }

        const auto running() const { return m_running; }

        WorkerState worker_state(int id) const { return m_worker_state[id]; }

        const char* state_str() const {
            switch (m_state) {
            case WorkerState::Rendering: return "Running";
            case WorkerState::Paused: return "Paused";
            case WorkerState::Stopped: return "Stopped";
            }

            return "";
        }

        const auto samples_done() const { return m_samples_done - 1; }

        void generate_blocks() {
            std::lock_guard g(m_blocks_mtx);

            m_blocks.clear();

            for (int y = 0; y < m_film.height(); y += 32)
                for (int x = 0; x < m_film.width(); x += 32) {
                    m_blocks.push_back(Block{x, y, std::min(m_film.width(), x + 32),
                                             std::min(m_film.height(), y + 32)});
                }

            m_samples_done++;
        }

        bool has_block() const { return m_blocks.size() > 0; }

    private:
        std::optional<Block> aquire_block() {
            std::lock_guard g(m_blocks_mtx);

            if (m_blocks.size() == 0)
                return std::nullopt;

            auto first = m_blocks.front();
            m_blocks.erase(m_blocks.begin());

            return first;
        }

        void render_block(Block block) {
            for (int y = block.start_y; y < block.end_y; y++)
                for (int x = block.start_x; x < block.end_x; x++) {
                    auto camray = m_camera.get_ray(x, y, m_film.width(), m_film.height());
                    m_film.splat(x, y, m_integrator->integrate(camray));
                }
        }

    private:
        Camera m_camera;

        SampleFilm  m_film;
        Integrator* m_integrator;

        std::vector<Block> m_blocks;
        std::mutex         m_blocks_mtx;

        bool        m_running;
        WorkerState m_state;

        int m_samples_done = 0;

        std::vector<std::thread> m_workers;
        std::vector<WorkerState> m_worker_state;
    };

} // namespace Oxy::Renderer
