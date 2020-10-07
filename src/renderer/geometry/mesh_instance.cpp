#include "renderer/geometry/mesh_instance.hpp"

namespace Oxy::Renderer {

    bool MeshInstance::intersect_ray(const glm::dvec3& origin, const glm::dvec3& dir,
                                     IntersectionResult& res) const {

        return m_instanced_mesh->intersect_ray(origin, dir, res);
    }

} // namespace Oxy::Renderer
