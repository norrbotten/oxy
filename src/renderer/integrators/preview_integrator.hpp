#pragma once

#include "renderer/integrators/integrator.hpp"

namespace Oxy::Renderer {

    class PreviewIntegrator final : public Integrator {
    public:
        PreviewIntegrator(int width, int height, SampleFilm& film);
        virtual ~PreviewIntegrator() override;

        virtual Color integrate(const CameraRay& camray) const override;

    private:
    };

} // namespace Oxy::Renderer
