#include "sample_film.hpp"

namespace Oxy::Renderer {

    SampleFilm::SampleFilm()
        : m_width(0)
        , m_height(0)
        , m_cumulative_buffer(nullptr)
        , m_sample_count(nullptr) {}

    SampleFilm::~SampleFilm() {
        if (m_cumulative_buffer != nullptr)
            delete[] m_cumulative_buffer;

        if (m_sample_count != nullptr)
            delete[] m_sample_count;
    }

    void SampleFilm::resize(int width, int height) {
        if (width > 0 && height > 0 && width < 16384 && height < 16384) {
            m_width  = width;
            m_height = height;

            if (m_cumulative_buffer != nullptr)
                delete[] m_cumulative_buffer;

            if (m_sample_count != nullptr)
                delete[] m_sample_count;

            m_cumulative_buffer = new double[3 * width * height]();
            m_sample_count      = new unsigned int[width * height]();
        }
    }

    void SampleFilm::splat(int x, int y, double r, double g, double b) {
        if (x >= 0 && y >= 0 && x < m_width && y < m_height) {
            auto ofs = 3 * (x + y * m_width);

            m_cumulative_buffer[ofs + 0] += r;
            m_cumulative_buffer[ofs + 1] += g;
            m_cumulative_buffer[ofs + 2] += b;

            m_sample_count[x + y * m_width]++;
        }
    }

    void SampleFilm::splat(int x, int y, const Color& col) {
        splat(x, y, col.r(), col.g(), col.b());
    }

    Color SampleFilm::get(int x, int y, double exposure) const {
        auto expose = [](double val, double fac) -> double { return std::pow(val, fac) * fac; };

        if (x >= 0 && y >= 0 && x < m_width && y < m_height) {
            auto num_samples = m_sample_count[x + y * m_width];
            if (num_samples == 0)
                return Color();

            double r = m_cumulative_buffer[3 * (x + y * m_width) + 0] / num_samples;
            double g = m_cumulative_buffer[3 * (x + y * m_width) + 1] / num_samples;
            double b = m_cumulative_buffer[3 * (x + y * m_width) + 2] / num_samples;

            return {expose(r, exposure), expose(g, exposure), expose(b, exposure)};
        }

        return Color();
    }

} // namespace Oxy::Renderer
