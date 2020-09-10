#pragma once

#include "renderer/accel/accelerator.hpp"
#include "renderer/utils/color.hpp"

namespace Oxy::Renderer {

    enum class Integrators {
        Fractal,
    };

    class Integrator {
    public:
        Integrator();
        virtual ~Integrator() = default;

        virtual Color integrate(int x, int y) = 0;

        auto& accel() { return m_accel; }

    private:
        Accelerator m_accel;
    };

} // namespace Oxy::Renderer
