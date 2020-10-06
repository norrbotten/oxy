#include "renderer/renderer.hpp"

#include "renderer/geometry/mesh.hpp"
#include "renderer/geometry/mesh_instance.hpp"

namespace Oxy::Renderer {

    OxyRenderer::OxyRenderer()
        : m_scene(m_ctx)
        , m_running(false)
        , m_state(WorkerState::Stopped) {

        auto model = new Mesh("./baby_yoda.stl");

        auto model_transform = glm::translate(glm::dmat4(1.0), glm::dvec3(0, 0, 0));
        model->set_transform(model_transform);

        for (int y = -200; y < 200; y += 100) {
            for (int x = -200; x < 200; x += 100) {
                auto instance = new MeshInstance(model);
                instance->set_transform(glm::translate(glm::dmat4(1.0), glm::dvec3(x, y, 0.0)));
                m_scene.add_object(instance);
            }
        }

        model->setup();

        m_scene.setup();

        camera().set_fov(50);
        camera().set_pos(glm::dvec3(-350, -350, 250));
        camera().aim(glm::dvec3(0, 0, 16));

        m_film.clear();
    }

    void OxyRenderer::set_render_resolution(int width, int height) {
        m_ctx.width  = width;
        m_ctx.height = height;
        m_film.resize(width, height);
    }

    void OxyRenderer::select_integrator() {}

    void OxyRenderer::start_render(unsigned int num_threads) {
        if (!m_continous_sampling && m_samples_done > m_samples_to_do)
            return;

        m_running = true;
        m_state   = WorkerState::Rendering;

        for (unsigned int i = 0; i < m_worker_state.size(); i++)
            m_worker_state[i] = WorkerState::Rendering;

        if (num_threads > m_workers.size()) {
            for (unsigned int i = m_worker_state.size(); i < num_threads; i++) {
                auto id = m_workers.size();

                auto worker_func = [&](int id) -> void {
                    while (true) {
                        auto state = this->worker_state(id);
                        if (state == WorkerState::Stopped)
                            break;

                        if (state == WorkerState::Rendering)
                            if (auto block = this->aquire_block(); block.has_value())
                                this->render_block(block.value());

                        if (state == WorkerState::Paused) {
                            using namespace std::chrono_literals;
                            std::this_thread::sleep_for(10ms);
                        }
                        else
                            std::this_thread::yield();
                    }
                };

                m_worker_state.push_back(WorkerState::Rendering);
                m_workers.push_back(std::thread(worker_func, id));
            }
        }
    }

    void OxyRenderer::pause_render() {
        m_running = false;
        m_state   = WorkerState::Paused;

        for (unsigned int i = 0; i < m_worker_state.size(); i++)
            m_worker_state[i] = WorkerState::Paused;
    }

    void OxyRenderer::reset_render() {
        m_running = false;
        m_state   = WorkerState::Stopped;

        for (unsigned int i = 0; i < m_worker_state.size(); i++)
            m_worker_state[i] = WorkerState::Stopped;

        for (auto& thread : m_workers)
            if (thread.joinable())
                thread.join();

        m_worker_state.clear();
        m_workers.clear();

        m_film.clear();

        m_blocks.clear();
        m_samples_done = 0;
    }

    const char* OxyRenderer::state_str() const {
        switch (m_state) {
        case WorkerState::Rendering: return "Running";
        case WorkerState::Paused: return "Paused";
        case WorkerState::Stopped: return "Stopped";
        }
        return "";
    }

    void OxyRenderer::next_sample() {
        if (has_block())
            return;

        if (!m_continous_sampling && m_samples_done >= m_samples_to_do)
            pause_render();

        std::lock_guard g(m_blocks_mtx);

        m_blocks.clear();

        for (int y = 0; y < m_film.height(); y += 32)
            for (int x = 0; x < m_film.width(); x += 32) {
                m_blocks.push_back(Block{x, y, std::min(m_film.width(), x + 32),
                                         std::min(m_film.height(), y + 32)});
            }

        m_samples_done++;
    }

} // namespace Oxy::Renderer
