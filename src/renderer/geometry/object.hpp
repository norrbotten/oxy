#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

#include "renderer/utils/intersection_result.hpp"

namespace Oxy::Renderer {

    class Object {
    public:
        virtual ~Object() = default;

        virtual bool setup() { return false; }

        virtual bool intersects_ray(const glm::dvec3& origin, const glm::dvec3& dir,
                                    IntersectionResult& res) const = 0;

        void set_transform(const glm::dmat4& transform) {
            m_transform     = transform;
            m_inv_transform = glm::inverse(m_transform);

            glm::dvec3 scale, translation, skew;
            glm::dvec4 perspective;
            glm::decompose(m_transform, scale, m_quat, translation, skew, perspective);
            m_quat = glm::conjugate(m_quat);

            m_inv_quat = glm::inverse(m_quat);
        }

        inline glm::dvec3 world_to_local(const glm::dvec3& world) {
            return m_inv_transform * glm::dvec4(world, 1);
        }

        inline glm::dvec3 local_to_world(const glm::dvec3& local) {
            return m_transform * glm::dvec4(local, 1);
        }

        inline glm::dvec3 world_to_local_dir(const glm::dvec3& world_dir) {
            return m_inv_quat * world_dir;
        }

        inline glm::dvec3 local_to_world_dir(const glm::dvec3& local_dir) {
            return m_inv_quat * local_dir;
        }

    private:
        glm::dmat4 m_transform     = glm::dmat4();
        glm::dmat4 m_inv_transform = glm::dmat4();

        glm::dquat m_quat     = glm::dquat();
        glm::dquat m_inv_quat = glm::dquat();
    };

} // namespace Oxy::Renderer
