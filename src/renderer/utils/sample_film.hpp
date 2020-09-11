#pragma once

#include <iostream>

#include "renderer/utils/color.hpp"

namespace Oxy::Renderer {

    class SampleFilm final {
    public:
        SampleFilm();
        ~SampleFilm();

        auto width() const { return m_width; }
        auto height() const { return m_height; }

        void resize(int width, int height);
        void splat(int x, int y, double r, double g, double b);
        void splat(int x, int y, const Color& sample);

        Color get(int x, int y, double exposure = 1.0) const;

        void copy_to_rgba_buffer(char* buffer, double exposure = 1.0) const {
            for (int y = 0; y < m_height; y++)
                for (int x = 0; x < m_width; x++) {
                    char r, g, b, a = (char)255;
                    auto col = get(x, y, exposure);
                    col.to_chars(r, g, b);

                    *(buffer + 0) = r;
                    *(buffer + 1) = g;
                    *(buffer + 2) = b;
                    *(buffer + 3) = a;

                    buffer += 4;
                }
        }

        void clear() {
            if (m_cumulative_buffer != nullptr)
                for (unsigned int i = 0; i < 3 * m_width * m_height; i++)
                    m_cumulative_buffer[i] = 0.0;

            if (m_sample_count != nullptr)
                for (unsigned int i = 0; i < m_width * m_height; i++)
                    m_sample_count[i] = 0;
        }

    private:
        Color get_cumulative(int x, int y) const {
            return {m_cumulative_buffer[3 * (x + y * m_width) + 0],
                    m_cumulative_buffer[3 * (x + y * m_width) + 1],
                    m_cumulative_buffer[3 * (x + y * m_width) + 2]};
        }
        unsigned int get_samples(int x, int y) const { return m_sample_count[x + y * m_width]; }

    private:
        int           m_width, m_height;
        double*       m_cumulative_buffer;
        unsigned int* m_sample_count;
    };

} // namespace Oxy::Renderer
