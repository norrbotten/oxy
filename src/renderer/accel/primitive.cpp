#include "renderer/accel/primitive.hpp"

namespace Oxy::Renderer {

    bool Triangle::intersect_ray(const glm::dvec3& orig, const glm::dvec3& dir, double& t) const {
        auto v0v1 = m_p1 - m_p0;
        auto v0v2 = m_p2 - m_p0;
        auto pvec = glm::cross(dir, v0v2);

        auto det = glm::dot(v0v1, pvec);

        if (std::fabs(det) < 1e-9)
            return false;

        auto inv_det = 1.0 / det;

        auto tvec = orig - m_p0;
        auto qvec = glm::cross(tvec, v0v1);

        auto u = glm::dot(tvec, pvec) * inv_det;
        auto v = glm::dot(dir, qvec) * inv_det;

        if ((u < 0) | (u > 1) | (v < 0) | (u + v > 1))
            return false;

        t = glm::dot(v0v2, qvec) * inv_det;

        return true;
    }

    bool Sphere::intersect_ray(const glm::dvec3& orig, const glm::dvec3& dir, double& t) const {
        auto oc   = orig - m_center;
        auto a    = glm::dot(dir, dir);
        auto b    = 2.0 * glm::dot(oc, dir);
        auto c    = glm::dot(oc, oc) - m_radius * m_radius;
        auto disc = b * b - 4 * a * c;

        if (disc < 0)
            return false;

        auto t1 = (-b + std::sqrt(disc)) / (2.0 * a);
        auto t2 = (-b - std::sqrt(disc)) / (2.0 * a);

        if (t1 < 0) // inside the sphere, return the far point
            t = t2;
        else
            t = std::min(t1, t2);

        return true;
    }

    BoundingBox get_transformed_bbox(const BoundingBox& bbox, const glm::dmat4& transform) {
        auto& [min, max] = bbox;

        // 8 vertices of the bbox cube
        /*
        min x, min y, min z
        max x, min y, min z
        min x, max y, min z
        min x, min y, max x
        max x, max y, min z
        min x, max y, max z
        max x, min y, max z
        max x, max y, max z
        */

        /* clang-format off */
        glm::dvec3 points[8] = {
            {min.x, min.y, min.z},
            {max.x, min.y, min.z},
            {min.x, max.y, min.z},
            {min.x, min.y, max.z},
            {max.x, max.y, min.z},
            {min.x, max.y, max.z},
            {max.x, min.y, max.z},
            {max.x, max.y, max.z},
        };
        /* clang-format on */

        for (int i = 0; i < 8; i++)
            points[i] = transform * glm::dvec4(points[i], 1.0);

        glm::dvec3 new_min(std::numeric_limits<double>::max());
        glm::dvec3 new_max(std::numeric_limits<double>::lowest());

        for (int i = 0; i < 8; i++) {
            for (int axis = 0; axis < 3; axis++) {
                new_min[axis] = std::min(new_min[axis], points[i][axis]);
                new_max[axis] = std::max(new_max[axis], points[i][axis]);
            }
        }

        // theres probably a better way of doing this

        return {new_min, new_max};
    }

    BoundingSphere get_transformed_bsphere(const BoundingSphere& bsphere,
                                           const glm::dmat4&     transform) {
        return {transform * glm::dvec4(bsphere.first, 1.0), bsphere.second};
    }

} // namespace Oxy::Renderer
