#pragma once

#include <glm/glm.hpp>

namespace Oxy::Renderer {

    struct IntersectionResult {
        bool   hit;
        double t;

        glm::dvec3 hitpos;
        glm::dvec3 hitnormal;
    };

} // namespace Oxy::Renderer
