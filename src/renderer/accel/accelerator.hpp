#pragma once

#include <numeric>

#include <glm/glm.hpp>

#include "renderer/accel/bvh.hpp"
#include "renderer/utils/camera.hpp"
#include "renderer/utils/intersection_result.hpp"

namespace Oxy::Renderer {

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
