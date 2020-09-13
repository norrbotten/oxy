#include "renderer/renderer.hpp"

#include "renderer/parsers/stl.hpp"

namespace Oxy::Renderer {

    OxyRenderer::OxyRenderer()
        : m_integrator(nullptr)
        , m_running(false)
        , m_state(WorkerState::Stopped) {}

    OxyRenderer::~OxyRenderer() {
        if (m_integrator != nullptr)
            delete m_integrator;
    }

    void OxyRenderer::set_render_resolution(int width, int height) {
        if (m_integrator != nullptr)
            m_integrator->set_resolution(width, height);

        m_film.resize(width, height);
    }

    void OxyRenderer::select_integrator(Integrators integrator) {
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

        if (auto err = Parsers::parse_stl("./uwu.stl", triangles); err.has_value()) {
            std::cout << err.value() << "\n";
        }
        else {
            std::cout << triangles.size() << " triangles\n";
        }

        m_integrator->accel().triangle_bvh().build(triangles);

        m_film.clear();
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

    const char* OxyRenderer::state_str() const {
        switch (m_state) {
        case WorkerState::Rendering: return "Running";
        case WorkerState::Paused: return "Paused";
        case WorkerState::Stopped: return "Stopped";
        }

        return "";
    }

    void OxyRenderer::generate_blocks() {
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
