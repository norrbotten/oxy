#include "renderer/integrators/preview_integrator.hpp"

namespace Oxy::Renderer {

    PreviewIntegrator::PreviewIntegrator(int width, int height, SampleFilm& film)
        : Integrator(width, height, film) {}

    PreviewIntegrator::~PreviewIntegrator() {}

    Color PreviewIntegrator::integrate(CameraRay camray) const {
        IntersectionResult res;

        if (m_accel.intersect_ray(camray.origin, camray.dir, res)) {
            // return Color::from_normal(res.hitnormal);
            return Color(glm::abs(glm::dot(camray.dir, res.hitnormal)));
        }

        return Color();
    }

} // namespace Oxy::Renderer
