#pragma once

#include <utility>

#include <glm/glm.hpp>

namespace Oxy::Renderer {

    enum class Primitives {
        Triangle,
        Sphere,
    };

    template <Primitives PrimitiveType>
    class Primitive final {
        std::pair<glm::dvec3, glm::dvec3> get_bbox() const;
    };

    template <>
    class Primitive<Primitives::Triangle> final {
    public:
        Primitive(glm::dvec3 p0, glm::dvec3 p1, glm::dvec3 p2)
            : m_p0(p0)
            , m_p1(p1)
            , m_p2(p2) {

            compute_normal();
            compute_bbox();
        }

        Primitive(const glm::dvec3& p0, const glm::dvec3& p1, const glm::dvec3& p2)
            : m_p0(p0)
            , m_p1(p1)
            , m_p2(p2) {

            compute_normal();
            compute_bbox();
        }

        const auto& p0() const { return m_p0; }
        const auto& p1() const { return m_p1; }
        const auto& p2() const { return m_p2; }
        const auto& normal() const { return m_normal; }

        std::pair<glm::dvec3, glm::dvec3> bbox() const { return {m_box_min, m_box_max}; }

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

    private:
        glm::dvec3 m_p0, m_p1, m_p2;
        glm::dvec3 m_normal;

        glm::dvec3 m_box_min, m_box_max;
    };

    template <>
    class Primitive<Primitives::Sphere> final {
    public:
        Primitive(glm::dvec3 center, double radius)
            : m_center(center)
            , m_radius(radius) {}

        Primitive(const glm::dvec3& center, double radius)
            : m_center(center)
            , m_radius(radius) {}

        const auto normal(glm::dvec3 point) { return (point - m_center) / m_radius; }

        std::pair<glm::dvec3, glm::dvec3> bbox() const {
            return {m_center - glm::dvec3(m_radius), m_center + glm::dvec3(m_radius)};
        }

    private:
        glm::dvec3 m_center;
        double     m_radius;
    };

    using Triangle = Primitive<Primitives::Triangle>;
    using Sphere   = Primitive<Primitives::Sphere>;

} // namespace Oxy::Renderer
