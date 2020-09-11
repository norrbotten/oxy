#pragma once

#include "renderer/accel/accelerator.hpp"
#include "renderer/utils/camera.hpp"
#include "renderer/utils/color.hpp"
#include "renderer/utils/random.hpp"
#include "renderer/utils/sample_film.hpp"

namespace Oxy::Renderer {

    enum class Integrators {
        Fractal,
        Buddhabrot,
    };

    class Integrator {
    public:
        Integrator(int width, int height, SampleFilm& film)
            : m_width(width)
            , m_height(height)
            , m_film(film) {}

        virtual ~Integrator() = default;

        auto& accel() { return m_accel; }

        virtual Color integrate(const CameraRay& camray) = 0;

        void set_resolution(int width, int height) {
            m_width  = width;
            m_height = height;
        }

    protected:
        Accelerator m_accel;
        int         m_width, m_height;
        SampleFilm& m_film;
    };

    class FractalIntegrator final : public Integrator {
    public:
        FractalIntegrator(int width, int height, SampleFilm& film)
            : Integrator(width, height, film) {}

        virtual ~FractalIntegrator() override {}

        virtual Color integrate(const CameraRay& camray) override;
    };

    class BuddhabrotIntegrator final : public Integrator {
    public:
        BuddhabrotIntegrator(int width, int height, SampleFilm& film)
            : Integrator(width, height, film) {}

        virtual ~BuddhabrotIntegrator() override {}

        virtual Color integrate(const CameraRay& camray) override;
    };

} // namespace Oxy::Renderer
