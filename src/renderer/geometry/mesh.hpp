#pragma once

#include <string>
#include <vector>

#include "renderer/geometry/object.hpp"

#include "renderer/accel/bvh.hpp"
#include "renderer/accel/primitive.hpp"

#include "renderer/parsers/stl.hpp"

namespace Oxy::Renderer {

    class Mesh final : public Object {
    public:
        Mesh(const std::string& filename);
        Mesh(const std::vector<Triangle>& tris);
        ~Mesh();

        virtual bool intersects_ray(const glm::dvec3& origin, const glm::dvec3& dir,
                                    IntersectionResult& res) const override;

        virtual bool setup() override;

    private:
        bool m_errored;

        UnoptimizedBVHNode<Triangle>* m_bvh;
        std::vector<Triangle>         m_triangles;
    };

} // namespace Oxy::Renderer
