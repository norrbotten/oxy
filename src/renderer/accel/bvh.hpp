#pragma once

#include <immintrin.h>
#include <vector>

#include <glm/glm.hpp>

#include "renderer/accel/primitive.hpp"
#include "renderer/utils/intersection_result.hpp"

namespace Oxy::Renderer {

    // this is obviously not a bsp, cant bother implementing it yet

    const __m256 one_m256       = _mm256_set1_ps(1.0f);
    const __m256 minus_one_m256 = _mm256_set1_ps(-1.0f);
    const __m256 pos_eps_m256   = _mm256_set1_ps(1e-9f);
    const __m256 neg_eps_m256   = _mm256_set1_ps(-1e-9f);
    const __m256 zero_m256      = _mm256_set1_ps(0.0f);
    const __m256 fmax_m256      = _mm256_set1_ps(std::numeric_limits<float>::max());

    inline void avx2_cross(__m256 result[3], const __m256 a[3], const __m256 b[3]) {
        result[0] = _mm256_fmsub_ps(a[1], b[2], _mm256_mul_ps(b[1], a[2]));
        result[1] = _mm256_fmsub_ps(a[2], b[0], _mm256_mul_ps(b[2], a[0]));
        result[2] = _mm256_fmsub_ps(a[0], b[1], _mm256_mul_ps(b[0], a[1]));
    }

    inline __m256 avx2_dot(const __m256 a[3], const __m256 b[3]) {
        return _mm256_fmadd_ps(a[2], b[2], _mm256_fmadd_ps(a[1], b[1], _mm256_mul_ps(a[0], b[0])));
    }

    inline void avx2_sub(__m256 result[3], const __m256 a[3], const __m256 b[3]) {
        result[0] = _mm256_sub_ps(a[0], b[0]);
        result[1] = _mm256_sub_ps(a[1], b[1]);
        result[2] = _mm256_sub_ps(a[2], b[2]);
    }

    inline float avx2_extract(const __m256 x, const int n) { return ((float*)(&x))[n]; }

    struct packed_triangles {
        __m256 edge1[3]; // v1 - v0
        __m256 edge2[3]; // v2 - v0
        __m256 vert0[3];
        __m256 inactive;

        // not used in intersection code, but still need normals
        __m256 normal[3];
    };

    struct packed_ray {
        __m256 origin[3];
        __m256 direction[3];
    };

    struct packed_spheres {
        __m256 center[3];
        __m256 radius;
        __m256 inactive;
    };

    template <Primitives T>
    class BVH final {
    public:
        bool intersect_ray(const glm::dvec3& origin, const glm::dvec3& dir,
                           IntersectionResult& res);

        void build(const std::vector<Primitive<T>>& primitives);
    };

    template <>
    class BVH<Primitives::Triangle> final {
    public:
        bool intersect_ray(const glm::dvec3& origin, const glm::dvec3& dir,
                           IntersectionResult& res);

        void build(const std::vector<Triangle>& triangles);

    private:
        std::vector<packed_triangles> m_triangles;
    };

    template <>
    class BVH<Primitives::Sphere> final {
    public:
        bool intersect_ray(const glm::dvec3& origin, const glm::dvec3& dir,
                           IntersectionResult& res);

        void build(const std::vector<Sphere>& spheres);
    };

} // namespace Oxy::Renderer
