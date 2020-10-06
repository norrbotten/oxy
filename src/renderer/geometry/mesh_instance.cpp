#include "renderer/geometry/mesh_instance.hpp"

namespace Oxy::Renderer {

    bool MeshInstance::intersects_ray(const glm::dvec3& origin, const glm::dvec3& dir,
                                      IntersectionResult& res) const {

        return m_instanced_mesh->intersects_ray(origin, dir, res);
    }

} // namespace Oxy::Renderer
