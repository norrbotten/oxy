#pragma once

#include <string>
#include <vector>

#include "renderer/geometry/object.hpp"

#include "renderer/accel/bvh.hpp"

namespace Oxy::Renderer {

    class Mesh final : public Object {
    public:
        Mesh(const std::string& filename);
        Mesh(const std::vector<Triangle>& tris);
        ~Mesh();

        virtual bool intersect_ray(const glm::dvec3& origin, const glm::dvec3& dir,
                                   IntersectionResult& res) const override;

        virtual std::pair<glm::dvec3, glm::dvec3> bbox() const override;

        virtual bool setup() override;

    private:
        bool m_errored;

        UnoptimizedBVHNode<Triangle>* m_bvh;
        std::vector<Triangle>         m_triangles;
    };

} // namespace Oxy::Renderer
