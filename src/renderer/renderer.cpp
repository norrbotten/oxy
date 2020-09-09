#include "renderer/renderer.hpp"

namespace Oxy::Renderer {

    OxyRenderer::OxyRenderer() {}

    void OxyRenderer::render_fractal() {
        auto width  = m_film.width();
        auto height = m_film.height();

        double aspect = (double)height / (double)width;

        constexpr int max_iters = 64;

        constexpr double radius      = 2.5;
        constexpr double real_offset = -0.7;
        constexpr double imag_offset = 0;

        auto fractal = [width, height, aspect](int x, int y) -> double {
            double cx = 2 * ((double)x / width - 0.5) * radius + real_offset;
            double cy = aspect * 2 * ((double)y / height - 0.5) * radius + imag_offset;

            double zx = 0, zy = 0;

            for (int i = 0; i < max_iters; i++) {
                auto temp = zx * zx - zy * zy + cx;
                zy        = 2 * zx * zy + cy;
                zx        = temp;

                if ((zx * zx + zy * zy) > 4)
                    return (double)i / max_iters;
            }

            return 1.0;
        };

        for (int y = 0; y < height; y++)
            for (int x = 0; x < width; x++) {
                auto frac = fractal(x, y);

                Color col;
                if (frac == 1.0)
                    col = {0, 0, 0};
                else
                    col = {frac, 0, 1 - frac};

                m_film.splat(x, y, col);
            }
    }

} // namespace Oxy::Renderer
