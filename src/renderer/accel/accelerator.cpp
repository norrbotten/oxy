#include "accelerator.hpp"

namespace Oxy::Renderer {

    bool Accelerator::intersect_ray(const glm::dvec3& origin, const glm::dvec3& dir,
                                    IntersectionResult& res) {

        double triangle_res = std::numeric_limits<double>::max();
        double sphere_res   = std::numeric_limits<double>::max();

        if (m_triangle_bvh.intersect_ray(origin, dir, triangle_res) ||
            m_sphere_bvh.intersect_ray(origin, dir, sphere_res)) {

            res.hit    = true;
            res.t      = std::min(triangle_res, sphere_res);
            res.hitpos = origin + dir * res.t;

            return true;
        }

        return false;
    }

    bool Accelerator::intersect_line(const glm::dvec3& start, const glm::dvec3& end,
                                     IntersectionResult& res) {

        auto dir = end - start;
        auto len = glm::length(dir);
        dir /= len;

        return intersect_ray(start, dir, res);
    }

} // namespace Oxy::Renderer
