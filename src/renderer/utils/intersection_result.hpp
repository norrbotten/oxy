#pragma once

#include <glm/glm.hpp>

namespace Oxy::Renderer {

    class Object; // forwarddecl

    struct IntersectionResult {
        bool   hit = false;
        double t   = std::numeric_limits<double>::max();

        glm::dvec3 hitpos{};
        glm::dvec3 hitnormal{};

        Object* hitobj = nullptr;
    };

} // namespace Oxy::Renderer
