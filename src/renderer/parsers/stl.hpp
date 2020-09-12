#pragma once

#include <fstream>
#include <optional>
#include <vector>

#include <glm/glm.hpp>

#include "renderer/accel/primitive.hpp"

namespace Oxy::Parsers {

    typedef std::optional<std::string> parse_error;

    // binary stl, much more sensible
    parse_error parse_stl(const char* filename, std::vector<Renderer::Triangle>& result) {
        struct stl_header {
            uint8_t  header[80];
            uint32_t num_tris;
        } __attribute__((packed));

        struct stl_vertex {
            float x, y, z;
        } __attribute__((packed));

        struct stl_triangle {
            float      nx, ny, nz;
            stl_vertex vert0;
            stl_vertex vert1;
            stl_vertex vert2;
            uint16_t   attrib;
        } __attribute__((packed));

        struct vertex {
            glm::dvec3 pos;
            glm::dvec3 normal;
            glm::dvec2 texcoord;
        };

        std::ifstream infile(filename, std::ios::binary);

        if (!infile.good())
            return "infile.good() == false";

        stl_header header;

        infile.read((char*)&header, sizeof(header));

        for (unsigned int i = 0; i < header.num_tris && !infile.eof(); i++) {
            stl_triangle tri;
            infile.read((char*)&tri, sizeof(tri));

            vertex vert1{glm::dvec3(tri.vert0.x, tri.vert0.y, tri.vert0.z),
                         glm::dvec3(tri.nx, tri.ny, tri.nz), glm::dvec2(0, 0)};

            vertex vert2{glm::dvec3(tri.vert1.x, tri.vert1.y, tri.vert1.z),
                         glm::dvec3(tri.nx, tri.ny, tri.nz), glm::dvec2(1, 0)};

            vertex vert3{glm::dvec3(tri.vert2.x, tri.vert2.y, tri.vert2.z),
                         glm::dvec3(tri.nx, tri.ny, tri.nz), glm::dvec2(1, 1)};

            result.push_back(Renderer::Triangle(vert1.pos, vert2.pos, vert3.pos));
        }

        return {};
    }

} // namespace Oxy::Parsers