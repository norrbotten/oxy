#pragma once

#include <utility>

#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

#include "renderer/utils/intersection_result.hpp"

#include "renderer/accel/bvh.hpp"
#include "renderer/accel/primitive_traits.hpp"

namespace Oxy::Renderer {

    class Object {
    public:
        virtual ~Object() = default;

        virtual bool setup() { return false; }

        virtual bool intersect_ray(const glm::dvec3& origin, const glm::dvec3& dir,
                                   IntersectionResult& res) const = 0;

        virtual BoundingBox bbox() const       = 0;
        virtual BoundingBox local_bbox() const = 0;

        virtual BoundingSphere bsphere() const       = 0;
        virtual BoundingSphere local_bsphere() const = 0;

        glm::dvec3 midpoint() const {
            auto [min, max] = bbox();
            return 0.5 * (min + max);
        };

        glm::dvec3 local_midpoint() const {
            auto [min, max] = local_bbox();
            return 0.5 * (min + max);
        };

        void set_transform(const glm::dmat4& transform) {
            m_transform     = transform;
            m_inv_transform = glm::inverse(m_transform);

            glm::dvec3 scale, translation, skew;
            glm::dvec4 perspective;
            glm::decompose(m_transform, scale, m_quat, translation, skew, perspective);
            m_quat = glm::conjugate(m_quat);

            m_inv_quat = glm::inverse(m_quat);
        }

        inline glm::dvec3 world_to_local(const glm::dvec3& world) const {
            return m_inv_transform * glm::dvec4(world, 1);
        }

        inline glm::dvec3 local_to_world(const glm::dvec3& local) const {
            return m_transform * glm::dvec4(local, 1);
        }

        inline glm::dvec3 world_to_local_dir(const glm::dvec3& world_dir) const {
            return m_inv_quat * world_dir;
        }

        inline glm::dvec3 local_to_world_dir(const glm::dvec3& local_dir) const {
            return m_inv_quat * local_dir;
        }

    protected:
        glm::dmat4 m_transform{1.0};
        glm::dmat4 m_inv_transform{1.0};

        glm::dquat m_quat{1.0, 0.0, 0.0, 0.0};
        glm::dquat m_inv_quat{1.0, 0.0, 0.0, 0.0};
    };

    template <>
    struct UnoptimizedBVHNode<Object*> {
        UnoptimizedBVHNode(std::vector<Object*>& prims)
            : primitives(prims) {}

        std::vector<Object*>& primitives;

        size_t left_index;
        size_t right_index;

        BoundingSphere bsphere;
        BoundingBox    bbox;

        UnoptimizedBVHNode<Object*>* left_node  = nullptr;
        UnoptimizedBVHNode<Object*>* right_node = nullptr;

        ~UnoptimizedBVHNode() {
            if (left_node != nullptr)
                delete left_node;

            if (right_node != nullptr)
                delete right_node;
        }
    };

    template <>
    inline std::pair<glm::dvec3, glm::dvec3> get_bbox(const std::vector<Object*>& primitives,
                                                      size_t start, size_t end) {

        glm::dvec3 min(std::numeric_limits<double>::max());
        glm::dvec3 max(std::numeric_limits<double>::lowest());

        for (auto it = primitives.begin() + start; it != primitives.begin() + end; it++) {
            auto [bbox_min, bbox_max] = (*it)->bbox();

            for (int axis = 0; axis < 3; axis++) {
                min[axis] = std::min(min[axis], bbox_min[axis]);
                max[axis] = std::max(max[axis], bbox_max[axis]);
            }
        }

        return {min, max};
    }

    template <>
    inline BoundingSphere get_bsphere(const std::vector<Object*>& primitives, size_t start,
                                      size_t end) {

        glm::dvec3 middle(0.0);

        for (auto it = primitives.begin() + start; it != primitives.begin() + end; it++) {
            middle += (*it)->midpoint();
        }

        middle /= (double)(end - start);

        double radius = 0;

        for (auto it = primitives.begin() + start; it != primitives.begin() + end; it++)
            radius = std::max(radius, glm::distance(middle, (*it)->midpoint()));

        return {middle, radius};
    }

    template <>
    inline UnoptimizedBVHNode<Object*>* build_bvh_generic(std::vector<Object*>& primitives,
                                                          size_t left_index, size_t right_index) {

        auto* node = new UnoptimizedBVHNode<Object*>{primitives};

        node->left_index  = left_index;
        node->right_index = right_index;

        auto [min_bbox, max_bbox] = get_bbox<Object*>(primitives, left_index, right_index);

        node->bsphere = get_bsphere<Object*>(primitives, left_index, right_index);
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
        std::sort(begin, end, [longest_axis](const Object* tri1, const Object* tri2) {
            return tri1->midpoint()[longest_axis] < tri2->midpoint()[longest_axis];
        });

        auto middle = (left_index + right_index) / 2;

        node->left_node  = build_bvh_generic<Object*>(primitives, left_index, middle);
        node->right_node = build_bvh_generic<Object*>(primitives, middle, right_index);

        return node;
    }

    inline bool dumb_bvh_traverse_objectptr(UnoptimizedBVHNode<Object*>* bvh,
                                            const std::vector<Object*>&  primitives,
                                            const glm::dvec3& origin, const glm::dvec3& dir,
                                            IntersectionResult& res) {
        IntersectionResult tmp_res;
        glm::dvec3         hitnormal;

        UnoptimizedBVHNode<Object*>* stack[2048] = {0};
        int                          stack_ptr   = 0;

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

                        IntersectionResult it_res;
                        if ((*it)->intersect_ray(origin, dir, it_res)) {
                            if (it_res.t < tmp_res.t) {
                                tmp_res.hit    = true;
                                tmp_res.t      = it_res.t;
                                tmp_res.hitobj = (*it);
                                hitnormal      = it_res.hitnormal;
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
            res.hitobj    = tmp_res.hitobj;
            res.hit       = tmp_res.hit;
            res.t         = tmp_res.t;
            res.hitnormal = hitnormal;

            return true;
        }

        return false;
    }

} // namespace Oxy::Renderer
