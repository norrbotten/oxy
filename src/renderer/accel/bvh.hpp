#pragma once

#include <glm/glm.hpp>

#include "renderer/accel/primitive.hpp"

namespace Oxy::Renderer {

    template <Primitives PrimitiveType>
    class BVH final {
    public:
        bool intersect_ray(const glm::dvec3& origin, const glm::dvec3& dir, double& t);
    };

    template <>
    class BVH<Primitives::Triangle> final {
    public:
        bool intersect_ray(const glm::dvec3& origin, const glm::dvec3& dir, double& t) {
            (void)origin;
            (void)dir;
            (void)t;
            return false;
        }
    };

    template <>
    class BVH<Primitives::Sphere> final {
    public:
        bool intersect_ray(const glm::dvec3& origin, const glm::dvec3& dir, double& t) {
            (void)origin;
            (void)dir;
            (void)t;
            return false;
        }
    };

} // namespace Oxy::Renderer
