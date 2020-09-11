#include "integrator.hpp"

namespace Oxy::Renderer {

    Color FractalIntegrator::integrate(const CameraRay& camray) {
        auto fractal = [](double cx, double cy) -> double {
            double zx = 0, zy = 0;

            for (int i = 0; i < 512; i++) {
                auto temp = zx * zx - zy * zy + cx;
                zy        = 2 * zx * zy + cy;
                zx        = temp;

                if ((zx * zx + zy * zy) > 4)
                    return (double)i / 512;
            }

            return 1.0;
        };

        auto aspect = (double)m_height / (double)m_width;

        auto tent_x = random<double>(0.0, 1.0);
        auto tent_y = random<double>(0.0, 1.0);

        double real_offset = -0.7463;
        double imag_offset = 0.1102;
        double radius      = 0.002;

        auto xf =
            radius * (((double)camray.origin.x + tent_x) / (double)m_width - 0.5) + real_offset;

        auto yf = radius * aspect * (((double)camray.origin.y + tent_y) / (double)m_width - 0.5) +
                  imag_offset;

        auto fac = fractal(xf, yf);

        Color col;
        if (fac == 1.0)
            col = {0, 0, 0};
        else
            col = {fac, 0, 0};

        return col;
    }

    Color BuddhabrotIntegrator::integrate(const CameraRay& camray) {
        double real_offset = -0.35;
        double imag_offset = 0;
        double radius      = 3;

        auto aspect = (double)m_height / (double)m_width;

        auto to_fractal = [&](double x, double y, double& fx, double& fy) {
            fx = radius * (x / m_width - 0.5) + real_offset;
            fy = radius * aspect * (y / m_height - 0.5) + imag_offset;
        };

        auto to_screen = [&](double fx, double fy, int& x, int& y) {
            x = (int)((double)m_width * ((fx - real_offset) / radius + 0.5));
            y = (int)((double)m_height * ((fy - imag_offset) / (radius * aspect) + 0.5));
        };

        auto tent_x = random<double>(0.0, 1.0);
        auto tent_y = random<double>(0.0, 1.0);

        double cx, cy;
        to_fractal((double)camray.origin.x + tent_x, (double)camray.origin.y + tent_y, cx, cy);

        double zx = 0, zy = 0;

        for (int i = 0; i < 256; i++) {
            auto temp = zx * zx - zy * zy + cx;
            zy        = 2 * zx * zy + cy;
            zx        = temp;

            if ((zx * zx + zy * zy) > 4)
                break;

            int x, y;
            to_screen(zx, zy, x, y);
            m_film.splat(x, y, 1, 0, 0);
        }

        return Color();
    }

} // namespace Oxy::Renderer
