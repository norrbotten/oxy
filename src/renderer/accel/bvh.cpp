#include "renderer/accel/bvh.hpp"

#include "renderer/utils/intersection_result.hpp"

namespace Oxy::Renderer {

    void BVH<Primitives::Triangle>::build(const std::vector<Triangle>& triangles) {
        const auto N = triangles.size();

        m_triangles.clear();
        m_triangles.resize(N / 8);

        for (unsigned int i = 0; i < N; i += 8) {
            glm::fvec3 edge1[8];
            glm::fvec3 edge2[8];
            glm::fvec3 vert0[8];
            glm::fvec3 normal[8];
            float      inactive[8];

            for (unsigned int n = 0; n < 8; n++) {
                edge1[n] = (i + n) < N ? glm::fvec3(triangles[i + n].p1() - triangles[i + n].p0())
                                       : glm::fvec3(0);
                edge2[n] = (i + n) < N ? glm::fvec3(triangles[i + n].p2() - triangles[i + n].p0())
                                       : glm::fvec3(0);
                vert0[n] = (i + n) < N ? glm::fvec3(triangles[i + n].p0()) : glm::fvec3(0);

                normal[n] = (i + n) < N ? glm::fvec3(triangles[i + n].normal()) : glm::fvec3(0);

                inactive[n] = (i + n) < N ? 0.f : 1.f;
            }

            packed_triangles tri;

            for (int n = 0; n < 3; n++) {
                tri.edge1[n] = _mm256_set_ps(edge1[0][n], edge1[1][n], edge1[2][n], edge1[3][n],
                                             edge1[4][n], edge1[5][n], edge1[6][n], edge1[7][n]);

                tri.edge2[n] = _mm256_set_ps(edge2[0][n], edge2[1][n], edge2[2][n], edge2[3][n],
                                             edge2[4][n], edge2[5][n], edge2[6][n], edge2[7][n]);

                tri.vert0[n] = _mm256_set_ps(vert0[0][n], vert0[1][n], vert0[2][n], vert0[3][n],
                                             vert0[4][n], vert0[5][n], vert0[6][n], vert0[7][n]);

                tri.normal[n] =
                    _mm256_set_ps(normal[0][n], normal[1][n], normal[2][n], normal[3][n],
                                  normal[4][n], normal[5][n], normal[6][n], normal[7][n]);
            }

            tri.inactive = _mm256_set_ps(inactive[0], inactive[1], inactive[2], inactive[3],
                                         inactive[4], inactive[5], inactive[6], inactive[7]);

            m_triangles.push_back(tri);
        }
    }

    bool BVH<Primitives::Triangle>::intersect_ray(const glm::dvec3& origin, const glm::dvec3& dir,
                                                  IntersectionResult& res) const {

        packed_ray p_ray;

        for (int n = 0; n < 3; n++) {
            p_ray.origin[n]    = _mm256_set1_ps((float)origin[n]);
            p_ray.direction[n] = _mm256_set1_ps((float)dir[n]);
        }

        for (auto& packed_tris : m_triangles) {

            // q = cross(ray.dir, v0v2)
            __m256 q[3];
            avx2_cross(q, p_ray.direction, packed_tris.edge2);

            // a = dot(v0v1, q)
            __m256 a = avx2_dot(packed_tris.edge1, q);

            // f = 1 / a
            __m256 f = _mm256_div_ps(one_m256, a);

            // s = ray.origin - v0
            __m256 s[3];
            avx2_sub(s, p_ray.origin, packed_tris.vert0);

            // u = f * dot(s, q)
            __m256 u = _mm256_mul_ps(f, avx2_dot(s, q));

            // r = cross(s, edge1)
            __m256 r[3];
            avx2_cross(r, s, packed_tris.edge1);

            // v = f * dot(ray.dir, r)
            __m256 v = _mm256_mul_ps(f, avx2_dot(p_ray.direction, r));

            // t = f * dot(v0v2, r)
            __m256 t = _mm256_mul_ps(f, avx2_dot(packed_tris.edge2, r));

            // mask of failed intersections
            __m256 failed;

            // t > eps && t < -eps
            failed = _mm256_and_ps(_mm256_cmp_ps(a, neg_eps_m256, _CMP_GT_OQ),
                                   _mm256_cmp_ps(a, pos_eps_m256, _CMP_LT_OQ));

            // u > 0
            failed = _mm256_or_ps(failed, _mm256_cmp_ps(u, zero_m256, _CMP_LT_OQ));

            // v > 0
            failed = _mm256_or_ps(failed, _mm256_cmp_ps(v, zero_m256, _CMP_LT_OQ));

            // (u + v) < 1
            failed = _mm256_or_ps(failed, _mm256_cmp_ps(_mm256_add_ps(u, v), one_m256, _CMP_GT_OQ));

            // tri.inactive == false
            failed = _mm256_or_ps(failed, packed_tris.inactive);

            // set failed ones to -1
            __m256 t_results = _mm256_blendv_ps(t, minus_one_m256, failed);

            // get failed isect mask as bitset in an int
            int mask = _mm256_movemask_ps(t_results);

            // at least 1 intersection
            if (mask != 0xFF) {
                for (int n = 0; n < 8; n++) {
                    float val = avx2_extract(t_results, n);

                    if (val > 0.f && val < res.t) {
                        res.t   = val;
                        res.hit = true;

                        res.hitpos    = origin + dir * res.t;
                        res.hitnormal = glm::dvec3(avx2_extract(packed_tris.normal[0], n),
                                                   avx2_extract(packed_tris.normal[1], n),
                                                   avx2_extract(packed_tris.normal[2], n));
                    }
                }
            }
        }

        return res.hit;
    }

    void BVH<Primitives::Sphere>::build(const std::vector<Sphere>& spheres) { (void)spheres; }

    bool BVH<Primitives::Sphere>::intersect_ray(const glm::dvec3& origin, const glm::dvec3& dir,
                                                IntersectionResult& res) const {

        (void)origin;
        (void)dir;
        (void)res;

        return false;
    }

} // namespace Oxy::Renderer
