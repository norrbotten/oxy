#pragma once

#include "renderer/accel/accelerator.hpp"
#include "renderer/utils/camera.hpp"
#include "renderer/utils/color.hpp"

namespace Oxy::Renderer {

    enum class Integrators {
        Fractal,
    };

    class Integrator {
    public:
        Integrator();
        virtual ~Integrator() = default;

        virtual Color integrate(const CameraRay& camray) = 0;

        auto& accel() { return m_accel; }

    private:
        Accelerator m_accel;
    };

    class FractalIntegrator final : public Integrator {
    public:
        FractalIntegrator() {}
        virtual ~FractalIntegrator() override {}

        virtual Color integrate(const CameraRay& camray) override {
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

            auto fac = fractal(camray.origin.x, camray.origin.y);

            Color col;
            if (fac == 1.0)
                col = {0, 0, 0};
            else
                col = {fac, 0, 0};

            return col;
        }
    };

} // namespace Oxy::Renderer
