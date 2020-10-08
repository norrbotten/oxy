#pragma once

#include <utility>

#include <glm/glm.hpp>

#include "renderer/accel/primitive_traits.hpp"

namespace Oxy::Renderer {

    using BoundingBox    = std::pair<glm::dvec3, glm::dvec3>;
    using BoundingSphere = std::pair<glm::dvec3, double>;

    enum class Primitives {
        Triangle,
        Sphere,
    };

    template <Primitives PrimitiveType>
    class Primitive final {};

    template <>
    class Primitive<Primitives::Triangle> final {
    public:
        Primitive(glm::dvec3 p0, glm::dvec3 p1, glm::dvec3 p2)
            : m_p0(p0)
            , m_p1(p1)
            , m_p2(p2) {

            compute_normal();
            compute_bbox();
            compute_midpoint();
        }

        const auto& p0() const { return m_p0; }
        const auto& p1() const { return m_p1; }
        const auto& p2() const { return m_p2; }

        const auto normal(const glm::dvec3&) const { return m_normal; }

        const auto& midpoint() const { return m_midpoint; }

        BoundingBox    bbox() const { return {m_box_min, m_box_max}; }
        BoundingSphere bsphere() const {
            auto middle = midpoint();
            auto radius =
                std::max(glm::distance(middle, p0()),
                         std::max(glm::distance(middle, p1()), glm::distance(middle, p2())));

            return {middle, radius * 2};
        }

        bool intersect_ray(const glm::dvec3& orig, const glm::dvec3& dir, double& t) const;

    private:
        inline void compute_normal() {
            auto edge1 = m_p1 - m_p0;
            auto edge2 = m_p2 - m_p0;
            m_normal   = glm::normalize(glm::cross(edge1, edge2));
        }

        inline void compute_bbox() {
            double min_x = std::min(m_p0.x, std::min(m_p1.x, m_p2.x));
            double min_y = std::min(m_p0.y, std::min(m_p1.y, m_p2.y));
            double min_z = std::min(m_p0.z, std::min(m_p1.z, m_p2.z));

            double max_x = std::max(m_p0.x, std::max(m_p1.x, m_p2.x));
            double max_y = std::max(m_p0.y, std::max(m_p1.y, m_p2.y));
            double max_z = std::max(m_p0.z, std::max(m_p1.z, m_p2.z));

            m_box_min = {min_x, min_y, min_z};
            m_box_max = {max_x, max_y, max_z};
        }

        inline void compute_midpoint() { m_midpoint = (m_p0 + m_p1 + m_p2) / 3.0; }

    private:
        glm::dvec3 m_p0, m_p1, m_p2;
        glm::dvec3 m_normal;

        glm::dvec3 m_box_min, m_box_max;
        glm::dvec3 m_midpoint;
    };

    template <>
    class Primitive<Primitives::Sphere> final {
    public:
        Primitive(glm::dvec3 center, double radius)
            : m_center(center)
            , m_radius(radius) {}

        const auto normal(glm::dvec3 point) const { return (point - m_center) / m_radius; }

        const auto& midpoint() const { return m_center; }

        BoundingBox bbox() const {
            return {m_center - glm::dvec3(m_radius), m_center + glm::dvec3(m_radius)};
        }

        BoundingSphere bsphere() const { return {m_center, m_radius}; }

        bool intersect_ray(const glm::dvec3& orig, const glm::dvec3& dir, double& t) const;

    private:
        glm::dvec3 m_center;
        double     m_radius;
    };

    using Triangle = Primitive<Primitives::Triangle>;
    using Sphere   = Primitive<Primitives::Sphere>;

    template <>
    inline BoundingBox PrimitiveTraits::bbox(Triangle tri) {
        return tri.bbox();
    }

    template <>
    inline BoundingSphere PrimitiveTraits::bsphere(Triangle tri) {
        return tri.bsphere();
    }

    template <>
    inline glm::dvec3 PrimitiveTraits::midpoint(Triangle tri) {
        return tri.midpoint();
    }

    template <>
    inline glm::dvec3 PrimitiveTraits::normal(Triangle tri, const glm::dvec3& hitpos) {
        return tri.normal(hitpos);
    }

    //

    template <>
    inline BoundingBox PrimitiveTraits::bbox(Sphere sph) {
        return sph.bbox();
    }

    template <>
    inline BoundingSphere PrimitiveTraits::bsphere(Sphere sph) {
        return sph.bsphere();
    }

    template <>
    inline glm::dvec3 PrimitiveTraits::midpoint(Sphere sph) {
        return sph.midpoint();
    }

    template <>
    inline glm::dvec3 PrimitiveTraits::normal(Sphere sph, const glm::dvec3& hitpos) {
        return sph.normal(hitpos);
    }

    BoundingBox    get_transformed_bbox(const BoundingBox& bbox, const glm::dmat4& transform);
    BoundingSphere get_transformed_bsphere(const BoundingSphere& bsphere,
                                           const glm::dmat4&     transform);

} // namespace Oxy::Renderer
