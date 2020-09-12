#include "renderer/renderer.hpp"

namespace Oxy::Renderer {

    OxyRenderer::OxyRenderer()
        : m_integrator(nullptr)
        , m_running(false)
        , m_state(WorkerState::Stopped) {}

    OxyRenderer::~OxyRenderer() {
        if (m_integrator != nullptr)
            delete m_integrator;
    }

    void OxyRenderer::start_render(unsigned int num_threads) {
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

} // namespace Oxy::Renderer
