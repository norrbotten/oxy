#pragma once

#include <algorithm>
#include <immintrin.h>
#include <iostream>
#include <vector>

#include <glm/glm.hpp>

#include "renderer/accel/primitive.hpp"
#include "renderer/utils/intersection_result.hpp"

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

    template <typename T>
    struct UnoptimizedBVHNode {
        UnoptimizedBVHNode(std::vector<T>& prims)
            : primitives(prims) {}

        std::vector<T>& primitives;

        size_t left_index;
        size_t right_index;

        glm::dvec3 bbox_min;
        glm::dvec3 bbox_max;

        UnoptimizedBVHNode<T>* right_node;
        UnoptimizedBVHNode<T>* left_node;

        ~UnoptimizedBVHNode() {
            delete right_node;
            delete left_node;
        }
    };

    template <typename T>
    std::pair<glm::dvec3, glm::dvec3> get_bbox(const std::vector<T>& primitives, size_t start,
                                               size_t end) {

        glm::dvec3 min(std::numeric_limits<double>::max());
        glm::dvec3 max(std::numeric_limits<double>::lowest());

        for (auto it = primitives.begin() + start; it != primitives.begin() + end; it++) {
            auto [bbox_min, bbox_max] = (*it).bbox();

            for (int axis = 0; axis < 3; axis++) {
                min[axis] = std::min(min[axis], bbox_min[axis]);
                max[axis] = std::max(max[axis], bbox_max[axis]);
            }
        }

        return {min, max};
    }

    template <typename T>
    UnoptimizedBVHNode<T>* build_bvh(std::vector<T>& primitives, size_t left_index,
                                     size_t right_index) {

        assert(right_index > left_index);

        std::cout << "left_index: " << left_index << ", right_index: " << right_index << "\n";

        if ((right_index - left_index) <= 8)
            return nullptr;

        auto* node = new UnoptimizedBVHNode<T>{primitives};

        node->primitives = primitives;

        node->left_index  = left_index;
        node->right_index = right_index;

        auto [min_bbox, max_bbox] = get_bbox<T>(primitives, left_index, right_index);

        node->bbox_min = min_bbox;
        node->bbox_max = max_bbox;

        const auto begin = primitives.begin() + left_index;
        const auto end   = primitives.begin() + right_index;

        // find the longest axis
        int    longest_axis = -1;
        double longest_len  = 0;

        for (int i = 0; i < 3; i++) {
            auto [min, max] = std::minmax_element(begin, end, [i](auto tri1, auto tri2) {
                return tri1.midpoint()[i] < tri2.midpoint()[i];
            });

            auto len = (*max).midpoint()[i] - (*min).midpoint()[i];

            if (len > longest_len) {
                longest_len  = len;
                longest_axis = i;
            }
        }

        assert(longest_axis != -1);

        // sort along the longest axis
        std::sort(begin, end, [longest_axis](auto tri1, auto tri2) {
            return tri1.midpoint()[longest_axis] < tri2.midpoint()[longest_axis];
        });

        auto middle = (left_index + right_index) / 2;

        std::cout << "recurse left\n";
        node->left_node = build_bvh<T>(primitives, left_index, middle);

        std::cout << "recurse right\n";
        node->right_node = build_bvh<T>(primitives, middle, right_index);

        return node;
    }

    template <Primitives T>
    class BVH final {
    public:
        bool intersect_ray(const glm::dvec3& origin, const glm::dvec3& dir,
                           IntersectionResult& res) const;

        void build(const std::vector<Primitive<T>>& primitives);
    };

    template <>
    class BVH<Primitives::Triangle> final {
    public:
        bool intersect_ray(const glm::dvec3& origin, const glm::dvec3& dir,
                           IntersectionResult& res) const;

        void build(const std::vector<Triangle>& triangles);

    private:
        std::vector<Triangle>         m_triangles;
        UnoptimizedBVHNode<Triangle>* m_bvh;
    };

    template <>
    class BVH<Primitives::Sphere> final {
    public:
        bool intersect_ray(const glm::dvec3& origin, const glm::dvec3& dir,
                           IntersectionResult& res) const;

        void build(const std::vector<Sphere>& spheres);

    private:
        std::vector<Sphere>         m_spheres;
        UnoptimizedBVHNode<Sphere>* m_bvh;
    };

} // namespace Oxy::Renderer
