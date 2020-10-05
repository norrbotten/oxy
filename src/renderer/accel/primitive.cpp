#include "primitive.hpp"

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

} // namespace Oxy::Renderer
