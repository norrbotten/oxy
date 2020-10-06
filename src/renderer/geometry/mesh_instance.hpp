#pragma once

#include "renderer/geometry/mesh.hpp"

namespace Oxy::Renderer {

    class MeshInstance final : public Object {
    public:
        MeshInstance(Mesh* mesh)
            : m_instanced_mesh(mesh) {}

        virtual bool intersects_ray(const glm::dvec3& origin, const glm::dvec3& dir,
                                    IntersectionResult& res) const override;

    private:
        Mesh* m_instanced_mesh;
    };

} // namespace Oxy::Renderer