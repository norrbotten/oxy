#include "renderer/geometry/mesh.hpp"

#include "renderer/parsers/stl.hpp"

namespace Oxy::Renderer {

    Mesh::Mesh(const std::string& filename)
        : m_errored(false)
        , m_bvh(nullptr) {

        if (filename.ends_with(".stl")) {
            auto err = Parsers::parse_stl(filename.c_str(), m_triangles);

            if (err.has_value())
                m_errored = true;
        }
        else {
            m_errored = true;
        }
    }

    Mesh::Mesh(const std::vector<Triangle>& triangles)
        : m_errored(false)
        , m_bvh(nullptr)
        , m_triangles(triangles) {}

    Mesh::~Mesh() {
        if (m_bvh != nullptr)
            delete m_bvh;
    }

    bool Mesh::setup() {
        if (m_errored)
            return false;

        m_bvh = build_bvh_generic<Triangle>(m_triangles, 0, m_triangles.size());

        return true;
    }

    bool Mesh::intersects_ray(const glm::dvec3& origin, const glm::dvec3& dir,
                              IntersectionResult& res) const {

        BVHTraverseResult bvh_res;

        if (dumb_bvh_traverse_generic<Triangle>(m_bvh, m_triangles, origin, dir, bvh_res)) {
            res.hit    = true;
            res.hitobj = (Object*)this;

            res.t         = bvh_res.t;
            res.hitnormal = bvh_res.hitnormal;
            res.hitpos    = origin + dir * bvh_res.t;

            return true;
        }

        return false;
    }

} // namespace Oxy::Renderer
