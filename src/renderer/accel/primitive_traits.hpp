#pragma once

#include <utility>

#include <glm/glm.hpp>

namespace Oxy::Renderer::PrimitiveTraits {

    using BoundingBox    = std::pair<glm::dvec3, glm::dvec3>;
    using BoundingSphere = std::pair<glm::dvec3, double>;

    template <typename T>
    BoundingBox bbox(T prim) = delete;

    template <typename T>
    BoundingBox bbox(T* prim) = delete;

    //

    template <typename T>
    BoundingSphere bsphere(T prim) = delete;

    template <typename T>
    BoundingSphere bsphere(T* prim) = delete;

    //

    template <typename T>
    glm::dvec3 midpoint(T prim) = delete;

    template <typename T>
    glm::dvec3 midpoint(T* prim) = delete;

    //

    template <typename T>
    glm::dvec3 normal(T prim, const glm::dvec3& hitpos) = delete;

    template <typename T>
    glm::dvec3 normal(T* prim, const glm::dvec3& hitpos) = delete;

} // namespace Oxy::Renderer::PrimitiveTraits
