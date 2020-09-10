#pragma once

#include <numeric>

#include <glm/glm.hpp>

#include "renderer/accel/bvh.hpp"

namespace Oxy::Renderer {

    struct IntersectionResult {
        bool   hit;
        double t;

        glm::dvec3 hitpos;
        glm::dvec3 hitnormal;
    };

    class Accelerator final {
    public:
        bool intersect_ray(const glm::dvec3& origin, const glm::dvec3& dir,
                           IntersectionResult& res);

        bool intersect_line(const glm::dvec3& start, const glm::dvec3& end,
                            IntersectionResult& res);

    private:
        BVH<Primitives::Triangle> m_triangle_bvh;
        BVH<Primitives::Sphere>   m_sphere_bvh;
    };

} // namespace Oxy::Renderer
