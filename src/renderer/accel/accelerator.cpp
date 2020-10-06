#include "accelerator.hpp"

/*

namespace Oxy::Renderer {

    bool Accelerator::intersect_ray(const glm::dvec3& origin, const glm::dvec3& dir,
                                    IntersectionResult& res) const {

        IntersectionResult results[2];

        m_triangle_bvh.intersect_ray(origin, dir, results[0]);
        m_sphere_bvh.intersect_ray(origin, dir, results[1]);

        for (int i = 0; i < 2; i++) {
            if (results[i].hit && results[i].t < res.t)
                res = results[i];
        }

        return res.hit;
    }

    bool Accelerator::intersect_line(const glm::dvec3& start, const glm::dvec3& end,
                                     IntersectionResult& res) const {

        auto dir = end - start;
        auto len = glm::length(dir);
        dir /= len;

        return intersect_ray(start, dir, res);
    }

} // namespace Oxy::Renderer
*/
