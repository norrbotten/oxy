#include "renderer/accel/bvh.hpp"

#include "renderer/utils/intersection_result.hpp"

#include <iostream>

namespace Oxy::Renderer {

    void BVH<Primitives::Triangle>::build(const std::vector<Triangle>& triangles) {
        m_triangles = triangles;
        m_bvh       = build_bvh_generic<Triangle>(m_triangles, 0, m_triangles.size());

        int bvh_buffer_size = (1 << ((int)std::log2(m_triangles.size()) + 1)) - 1;
        std::cout << bvh_buffer_size << "\n";
    }

    bool BVH<Primitives::Triangle>::intersect_ray(const glm::dvec3& origin, const glm::dvec3& dir,
                                                  IntersectionResult& res) const {

        return dumb_bvh_traverse_generic<Triangle>(m_bvh, m_triangles, origin, dir, res);
    }

    void BVH<Primitives::Sphere>::build(const std::vector<Sphere>& spheres) {
        m_spheres = spheres;
        m_bvh     = build_bvh_generic<Sphere>(m_spheres, 0, m_spheres.size());
    }

    bool BVH<Primitives::Sphere>::intersect_ray(const glm::dvec3& origin, const glm::dvec3& dir,
                                                IntersectionResult& res) const {

        return dumb_bvh_traverse_generic<Sphere>(m_bvh, m_spheres, origin, dir, res);
    }

} // namespace Oxy::Renderer
