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

} // namespace Oxy::Renderer
