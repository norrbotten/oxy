#pragma once

#include "renderer/geometry/mesh.hpp"

namespace Oxy::Renderer {

    class MeshInstance final : public Object {
    public:
        MeshInstance(Mesh* mesh)
            : m_instanced_mesh(mesh) {}

        virtual bool intersect_ray(const glm::dvec3& origin, const glm::dvec3& dir,
                                   IntersectionResult& res) const override;

        virtual BoundingBox bbox() const override { return m_instanced_mesh->bbox(); }

        virtual BoundingSphere bsphere() const override { return m_instanced_mesh->bsphere(); }

    private:
        Mesh* m_instanced_mesh;
    };

} // namespace Oxy::Renderer
