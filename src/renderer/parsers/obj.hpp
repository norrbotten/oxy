#pragma once

#include <algorithm>
#include <fstream>
#include <optional>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include <glm/glm.hpp>

#include "renderer/accel/primitive.hpp"

namespace Oxy::Parsers {

    typedef std::optional<std::string> parse_error;

    auto trim_string = [](std::string& to_trim) {
        to_trim.erase(to_trim.begin(), std::find_if(to_trim.begin(), to_trim.end(),
                                                    [](auto ch) { return !std::isspace(ch); }));
    };

    // the obj format is terrible
    // this is terrible and alot of the parsed data isnt even used, its from an old project
    // because writing obj parsers is about the most boring thing you can do
    parse_error parse_obj(const char* filename, std::vector<Renderer::Triangle>& result) {
        typedef std::tuple<double, double, double> obj_vertice;
        typedef std::tuple<double, double>         obj_texcoord;
        typedef std::tuple<double, double, double> obj_normal;

        struct obj_polygon {
            std::vector<int> vertex_indices;
            std::vector<int> texcoord_indices;
            std::vector<int> normal_indices;

            obj_polygon() {
                vertex_indices.reserve(3);
                texcoord_indices.reserve(3);
                normal_indices.reserve(3);
            }
        };

        struct vertex {
            glm::dvec3 pos;
            glm::dvec3 normal;
            glm::dvec2 texcoord;
        };

        std::ifstream infile(filename);

        if (!infile.good())
            return "infile.good() == false";

        std::vector<obj_vertice>  parsed_vertices;
        std::vector<obj_texcoord> parsed_texcoords;
        std::vector<obj_normal>   parsed_normals;

        int         line_no = 1;
        std::string line;

        auto fmt_error = [&line_no](const char* reason) -> std::string {
            return std::string("Obj parser failed on line ") + std::to_string(line_no) +
                   std::string(" - ") + std::string(reason);
        };

        auto try_stoi = [](std::string& str, int& result) -> bool {
            try {
                result = std::stoi(str);
                return false;
            }
            catch (...) {
                return true;
            }
        };

        while (std::getline(infile, line)) {
            trim_string(line);

            if (line.length() < 2)
                continue;

            std::stringstream ss(line);

            std::string first_word;
            if (ss >> first_word) {

                if (first_word == "v") {
                    double x, y, z, w;

                    if (!(ss >> x))
                        return fmt_error("invalid vertice x coord");

                    if (!(ss >> y))
                        return fmt_error("invalid vertice y coord");

                    if (!(ss >> z))
                        return fmt_error("invalid vertice x coord");

                    if (!(ss >> w))
                        w = 1;

                    parsed_vertices.push_back(std::make_tuple(x * w, z * w, y * w));
                }
                else if (first_word == "vt") {
                    double u, v;

                    if (!(ss >> u))
                        return fmt_error("invalid tex u coord");

                    if (!(ss >> v))
                        v = 0;

                    parsed_texcoords.push_back(std::make_tuple(u, v));
                }
                else if (first_word == "vn") {
                    double x, y, z;

                    if (!(ss >> x))
                        return fmt_error("invalid normal x value");

                    if (!(ss >> y))
                        return fmt_error("invalid normal y value");

                    if (!(ss >> z))
                        return fmt_error("invalid normal z value");

                    auto inv_len = 1.0 / std::hypot(x, y, z);
                    x *= inv_len;
                    y *= inv_len;
                    z *= inv_len;

                    parsed_normals.push_back(std::make_tuple(x, z, y));
                }
                else if (first_word == "vp") {
                    continue;
                }
                else if (first_word == "f") {
                    obj_polygon poly;

                    int classification = -1;

                    std::string facedecl;
                    while (ss >> facedecl) { // for each a/b/c declaration
                        int value1 = -1;
                        int value2 = -1;
                        int value3 = -1;

                        int parse_stage = 0;

                        std::string       val;
                        std::stringstream facedecl_ss(facedecl);

                        while (std::getline(facedecl_ss, val, '/')) { // for each value
                            if (parse_stage == 0) {
                                // first value must be present
                                if (val.length() == 0 || try_stoi(val, value1))
                                    return fmt_error("invalid facet declaration");
                            }
                            else if (parse_stage == 1) {
                                // second value can be omitted
                                if (val.length() > 0 && try_stoi(val, value2))
                                    return fmt_error("invalid facet declaration");
                            }
                            else if (parse_stage == 2) {
                                if (val.length() == 0 || try_stoi(val, value3))
                                    return fmt_error("invalid facet declaration");
                            }

                            parse_stage++;
                        }

                        // in case of more than just vertex index, third value must be present
                        if (parse_stage != 3 && value1 != -1 && value2 != -1) {
                            return fmt_error("invalid facet declaration");
                        }

                        // this checks so each a/b/c or whatever uses the same format
                        if (classification == -1) {
                            classification =
                                int(value1 != -1) << 2 | int(value2 != -1) << 1 | int(value3 != -1);
                        }
                        else {
                            int old_classification = classification;

                            classification =
                                int(value1 != -1) << 2 | int(value2 != -1) << 1 | int(value3 != -1);

                            if (old_classification != classification) {
                                return fmt_error("facet declarations are not the same format");
                            }
                        }

                        poly.vertex_indices.push_back(value1 - 1);

                        if (value2 != -1)
                            poly.texcoord_indices.push_back(value2 - 1);

                        if (value3 != -1)
                            poly.normal_indices.push_back(value3 - 1);
                    }

                    if (poly.vertex_indices.size() < 3) {
                        continue;
                    }

                    // triangulate the polygon and write them to the result
                    // TODO: place points on a common plane and do ear clipping

                    const bool has_texcoord = parsed_texcoords.size() > 0;
                    const bool has_normal   = parsed_normals.size() > 0;

                    auto calc_normal = [](glm::dvec3& p1, glm::dvec3& p2, glm::dvec3& p3) {
                        return glm::normalize(glm::cross(p3 - p1, p3 - p2));
                    };

                    const int n = poly.vertex_indices.size();

                    for (int i = 1; i < n - 1; i++) {
                        vertex vert0;
                        vertex vert1;
                        vertex vert2;

                        const auto v0 = poly.vertex_indices[0];
                        const auto v1 = poly.vertex_indices[i];
                        const auto v2 = poly.vertex_indices[i + 1];

                        // this is horrible code, but im not changing it from using tuples

                        vert0.pos = glm::dvec3(std::get<0>(parsed_vertices[v0]),
                                               std::get<1>(parsed_vertices[v0]),
                                               std::get<2>(parsed_vertices[v0]));

                        vert1.pos = glm::dvec3(std::get<0>(parsed_vertices[v1]),
                                               std::get<1>(parsed_vertices[v1]),
                                               std::get<2>(parsed_vertices[v1]));

                        vert2.pos = glm::dvec3(std::get<0>(parsed_vertices[v2]),
                                               std::get<1>(parsed_vertices[v2]),
                                               std::get<2>(parsed_vertices[v2]));

                        if (has_texcoord) {
                            const auto t0 = poly.texcoord_indices[0];
                            const auto t1 = poly.texcoord_indices[i];
                            const auto t2 = poly.texcoord_indices[i + 1];

                            vert0.texcoord = glm::dvec2(std::get<0>(parsed_texcoords[t0]),
                                                        std::get<1>(parsed_texcoords[t0]));

                            vert1.texcoord = glm::dvec2(std::get<0>(parsed_texcoords[t1]),
                                                        std::get<1>(parsed_texcoords[t1]));

                            vert2.texcoord = glm::dvec2(std::get<0>(parsed_texcoords[t2]),
                                                        std::get<1>(parsed_texcoords[t2]));
                        }
                        else {
                            vert0.texcoord = glm::dvec2(0, 0);
                            vert1.texcoord = glm::dvec2(1, 0);
                            vert2.texcoord = glm::dvec2(1, 1);
                        }

                        if (has_normal) {
                            const auto n0 = poly.normal_indices[0];
                            const auto n1 = poly.normal_indices[i];
                            const auto n2 = poly.normal_indices[i + 1];

                            vert0.normal = glm::dvec3(std::get<0>(parsed_normals[n0]),
                                                      std::get<1>(parsed_normals[n0]),
                                                      std::get<2>(parsed_normals[n0]));

                            vert1.normal = glm::dvec3(std::get<0>(parsed_normals[n1]),
                                                      std::get<1>(parsed_normals[n1]),
                                                      std::get<2>(parsed_normals[n1]));

                            vert2.normal = glm::dvec3(std::get<0>(parsed_normals[n2]),
                                                      std::get<1>(parsed_normals[n2]),
                                                      std::get<2>(parsed_normals[n2]));
                        }
                        else {
                            const auto norm = calc_normal(vert0.pos, vert1.pos, vert2.pos);
                            vert0.normal    = norm;
                            vert1.normal    = norm;
                            vert2.normal    = norm;
                        }

                        result.push_back(Renderer::Triangle(vert0.pos, vert1.pos, vert2.pos));
                    }
                }
                else if (first_word == "l") {
                    continue;
                }
            }

            line_no++;
        }

        return {};
    }

} // namespace Oxy::Parsers