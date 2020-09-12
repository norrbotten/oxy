#pragma once

#include <glm/glm.hpp>

namespace Oxy::Renderer {

    struct IntersectionResult {
        bool   hit = false;
        double t   = std::numeric_limits<double>::max();

        glm::dvec3 hitpos;
        glm::dvec3 hitnormal;
    };

} // namespace Oxy::Renderer
