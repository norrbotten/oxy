#pragma once

#include <algorithm>
#include <immintrin.h>
#include <iostream>
#include <numeric>
#include <vector>

#include <glm/glm.hpp>

#include "renderer/accel/primitive.hpp"

namespace Oxy::Renderer {

    inline bool ray_vs_aabb(const glm::dvec3& orig, const glm::dvec3& dir, const glm::dvec3& vmin,
                            const glm::dvec3& vmax, double& t) {

        auto rdx = 1.0 / dir.x;
        auto rdy = 1.0 / dir.y;
        auto rdz = 1.0 / dir.z;

        auto t1 = (vmin.x - orig.x) * rdx;
        auto t2 = (vmax.x - orig.x) * rdx;
        auto t3 = (vmin.y - orig.y) * rdy;
        auto t4 = (vmax.y - orig.y) * rdy;
        auto t5 = (vmin.z - orig.z) * rdz;
        auto t6 = (vmax.z - orig.z) * rdz;

        auto t7 = std::max(std::min(t1, t2), std::max(std::min(t3, t4), std::min(t5, t6)));
        auto t8 = std::min(std::max(t1, t2), std::min(std::max(t3, t4), std::max(t5, t6)));

        if (t8 < 0.0 || t8 < t7)
            return false;

        t = t7;

        return true;
    }

    inline bool ray_vs_sphere(const glm::dvec3& orig, const glm::dvec3& dir,
                              const glm::dvec3& center, double radius, double& t) {

        auto oc   = orig - center;
        auto a    = glm::dot(dir, dir);
        auto b    = 2.0 * glm::dot(oc, dir);
        auto c    = glm::dot(oc, oc) - radius * radius;
        auto disc = b * b - 4 * a * c;

        if (disc < 0)
            return false;

        auto t1 = (-b + std::sqrt(disc)) / (2.0 * a);
        auto t2 = (-b - std::sqrt(disc)) / (2.0 * a);

        if (t1 < 0) // inside the sphere, return the far point
            t = t2;
        else
            t = std::min(t1, t2);

        return true;
    }

    template <typename T>
    struct UnoptimizedBVHNode {
        UnoptimizedBVHNode(std::vector<T>& prims)
            : primitives(prims) {}

        std::vector<T>& primitives;

        size_t left_index;
        size_t right_index;

        BoundingSphere bsphere;
        BoundingBox    bbox;

        UnoptimizedBVHNode<T>* left_node  = nullptr;
        UnoptimizedBVHNode<T>* right_node = nullptr;

        ~UnoptimizedBVHNode() {
            if (left_node != nullptr)
                delete left_node;

            if (right_node != nullptr)
                delete right_node;
        }
    };

    template <typename T>
    BoundingBox get_bbox(const std::vector<T>& primitives, size_t start, size_t end) {
        glm::dvec3 min(std::numeric_limits<double>::max());
        glm::dvec3 max(std::numeric_limits<double>::lowest());

        for (auto it = primitives.begin() + start; it != primitives.begin() + end; it++) {
            auto [bbox_min, bbox_max] = PrimitiveTraits::bbox(*it);

            for (int axis = 0; axis < 3; axis++) {
                min[axis] = std::min(min[axis], bbox_min[axis]);
                max[axis] = std::max(max[axis], bbox_max[axis]);
            }
        }

        return {min, max};
    }

    template <typename T>
    BoundingSphere get_bsphere(const std::vector<T>& primitives, size_t start, size_t end) {
        glm::dvec3 middle(0.0);

        for (auto it = primitives.begin() + start; it != primitives.begin() + end; it++)
            middle += it->midpoint();

        middle /= (double)(end - start);

        double radius = 0;

        for (auto it = primitives.begin() + start; it != primitives.begin() + end; it++)
            radius = std::max(radius, glm::distance(middle, it->midpoint()) + it->bsphere().second);

        return {middle, radius};
    }

    template <typename T>
    UnoptimizedBVHNode<T>* build_bvh_generic(std::vector<T>& primitives, size_t left_index,
                                             size_t right_index) {

        auto* node = new UnoptimizedBVHNode<T>{primitives};

        node->left_index  = left_index;
        node->right_index = right_index;

        auto [min_bbox, max_bbox] = get_bbox<T>(primitives, left_index, right_index);

        node->bsphere = get_bsphere<T>(primitives, left_index, right_index);
        node->bbox    = {min_bbox, max_bbox};

        if ((right_index - left_index) <= 8) {
            return node;
        }

        const auto begin = primitives.begin() + left_index;
        const auto end   = primitives.begin() + right_index;

        // find the longest axis
        int    longest_axis = -1;
        double longest_len  = 0;

        for (int i = 0; i < 3; i++) {
            auto len = max_bbox[i] - min_bbox[i];

            if (len > longest_len) {
                longest_len  = len;
                longest_axis = i;
            }
        }

        assert(longest_axis != -1);

        // sort along the longest axis
        std::sort(begin, end, [longest_axis](auto tri1, auto tri2) {
            return PrimitiveTraits::midpoint(tri1)[longest_axis] <
                   PrimitiveTraits::midpoint(tri2)[longest_axis];
        });

        auto middle = (left_index + right_index) / 2;

        node->left_node  = build_bvh_generic<T>(primitives, left_index, middle);
        node->right_node = build_bvh_generic<T>(primitives, middle, right_index);

        return node;
    }

    struct BVHTraverseResult {
        bool       hit = false;
        double     t   = std::numeric_limits<double>::max();
        glm::dvec3 hitnormal;
    };

    template <typename T>
    bool dumb_bvh_traverse_generic(UnoptimizedBVHNode<T>* bvh, const std::vector<T>& primitives,
                                   const glm::dvec3& origin, const glm::dvec3& dir,
                                   BVHTraverseResult& res) {
        BVHTraverseResult tmp_res;
        glm::dvec3        hitnormal;

        UnoptimizedBVHNode<T>* stack[2048] = {0};
        int                    stack_ptr   = 0;

        stack[stack_ptr++] = bvh;

        while (stack_ptr != 0) {
            auto node = stack[--stack_ptr];

            double dummy;
            if (ray_vs_aabb(origin, dir, node->bbox.first, node->bbox.second, dummy)) {
                // if (ray_vs_sphere(origin, dir, node->bsphere.first, node->bsphere.second, dummy))
                // {
                bool is_leaf = (node->left_node == nullptr) && (node->right_node == nullptr);

                if (is_leaf) {
                    for (auto it = primitives.begin() + node->left_index;
                         it != primitives.begin() + node->right_index; it++) {

                        double t;
                        if (it->intersect_ray(origin, dir, t)) {
                            if (t < tmp_res.t) {
                                tmp_res.hit = true;
                                tmp_res.t   = t;
                                hitnormal   = PrimitiveTraits::normal(*it, origin + dir * t);
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

        if (tmp_res.hit) {
            res.hit       = tmp_res.hit;
            res.t         = tmp_res.t;
            res.hitnormal = hitnormal;

            return true;
        }

        return false;
    }

} // namespace Oxy::Renderer
