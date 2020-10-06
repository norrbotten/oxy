#pragma once

#include <chrono>
#include <iostream>
#include <mutex>
#include <optional>
#include <thread>
#include <vector>

#include "renderer/context.hpp"
#include "renderer/scene.hpp"

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

        auto get_render_width() const { return m_film.width(); }
        auto get_render_height() const { return m_film.height(); }

        void set_render_resolution(int width, int height);

        void select_integrator();

        void start_render(unsigned int num_threads = 24);
        void pause_render();
        void reset_render();

        void sample_continously(bool on) { m_continous_sampling = on; }
        void set_max_samples(int num_samples) { m_samples_to_do = num_samples; }

        const auto samples_done() const { return m_samples_done - 1; }

        float last_sample_time() const;
        float avg_sample_time() const;

        void next_sample();
        bool has_block() const { return m_blocks.size() > 0; }

        const auto& film() const { return m_film; }
        const auto& blocks() const { return m_blocks; }

        auto& camera() { return m_camera; }
        auto& scene() { return m_scene; }
        auto& ctx() { return m_ctx; }

        const char* state_str() const;
        const auto  running() const { return m_running; }

        WorkerState worker_state(int id) const { return m_worker_state[id]; }

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
                    m_film.splat(x, y, m_scene.get_sample(camray));
                }
        }

    private:
        RenderContext m_ctx;

        Camera     m_camera;
        SampleFilm m_film;
        Scene      m_scene;

        std::vector<Block> m_blocks;
        std::mutex         m_blocks_mtx;

        std::chrono::duration<double> m_last_sample_time;
        std::chrono::duration<double> m_avg_sample_time;

        bool        m_running;
        WorkerState m_state;

        int  m_samples_done       = 0;
        int  m_samples_to_do      = 1;
        bool m_continous_sampling = false;

        std::vector<std::thread> m_workers;
        std::vector<WorkerState> m_worker_state;
    };

} // namespace Oxy::Renderer
