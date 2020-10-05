#include "renderer/accel/bvh.hpp"

#include "renderer/utils/intersection_result.hpp"

#include <iostream>

namespace Oxy::Renderer {

    void BVH<Primitives::Triangle>::build(const std::vector<Triangle>& triangles) {
        m_triangles = triangles;
        m_bvh       = build_bvh<Triangle>(m_triangles, 0, m_triangles.size());
    }

    bool BVH<Primitives::Triangle>::intersect_ray(const glm::dvec3& origin, const glm::dvec3& dir,
                                                  IntersectionResult& res) const {

        IntersectionResult tmp_res;
        glm::dvec3         hitnormal;

        UnoptimizedBVHNode<Triangle>* stack[2048] = {0};
        int                           stack_ptr   = 0;

        stack[stack_ptr++] = m_bvh;

        while (stack_ptr != 0) {
            assert(stack_ptr <= 2048);

            auto node = stack[--stack_ptr];
            assert(node != nullptr);

            double dummy;
            if (ray_vs_aabb(origin, dir, node->bbox_min, node->bbox_max, dummy)) {
                bool is_leaf = (node->left_node == nullptr) && (node->right_node == nullptr);

                if (is_leaf) {
                    for (auto it = m_triangles.begin() + node->left_index;
                         it != m_triangles.begin() + node->right_index; it++) {

                        double t;
                        if (it->intersect_ray(origin, dir, t)) {
                            if (t < tmp_res.t) {
                                tmp_res.hit = true;
                                tmp_res.t   = t;
                                hitnormal   = it->normal();
                            }
                        }
                    }
                }
                else {
                    if (node->left_node != nullptr)
                        stack[stack_ptr++] = node->left_node;

                    if (node->right_node != nullptr)
                        stack[stack_ptr++] = node->right_node;
                }
            }
        }

        /*
        while (stack_ptr >= 0) {
            assert(stack_ptr < 2048);

            auto node = stack[stack_ptr--];
            assert(node != nullptr);

            bool is_leaf = (node->left_node == nullptr) && (node->right_node == nullptr);

            if (is_leaf) {
                for (auto it = m_triangles.begin() + node->left_index;
                     it != m_triangles.begin() + node->right_index; it++) {

                    double t;
                    if (it->intersect_ray(origin, dir, t)) {
                        if (t < tmp_res.t) {
                            tmp_res.hit = true;
                            tmp_res.t   = t;
                            hitnormal   = it->normal();
                        }
                    }
                }
            }
            else {
                double t_left       = std::numeric_limits<double>::max();
                bool   left_isected = false;

                double t_right       = std::numeric_limits<double>::max();
                bool   right_isected = false;

                left_isected = node->left_node != nullptr &&
                               ray_vs_aabb(origin, dir, node->left_node->bbox_min,
                                           node->left_node->bbox_max, t_left);

                right_isected = node->right_node != nullptr &&
                                ray_vs_aabb(origin, dir, node->right_node->bbox_min,
                                            node->right_node->bbox_max, t_right);

                if (left_isected && right_isected) {
                    if (t_left < t_right) {
                        stack[stack_ptr++] = node->left_node;
                        stack[stack_ptr++] = node->right_node;
                    }
                }
                else if (left_isected) {
                    stack[stack_ptr++] = node->left_node;
                }
                else if (right_isected) {
                    stack[stack_ptr++] = node->right_node;
                }
            }
        }
        */

        if (tmp_res.hit) {
            res.hit       = tmp_res.hit;
            res.t         = tmp_res.t;
            res.hitnormal = hitnormal;
            res.hitpos    = origin + dir * tmp_res.t;

            return true;
        }

        return false;
    }

    void BVH<Primitives::Sphere>::build(const std::vector<Sphere>& spheres) {
        m_spheres = spheres;
        m_bvh     = build_bvh<Sphere>(m_spheres, 0, m_spheres.size());
    }

    bool BVH<Primitives::Sphere>::intersect_ray(const glm::dvec3& origin, const glm::dvec3& dir,
                                                IntersectionResult& res) const {

        (void)origin;
        (void)dir;
        (void)res;

        return false;
    }

} // namespace Oxy::Renderer
