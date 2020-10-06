#pragma once

#include <random>

#include <glm/glm.hpp>

namespace Oxy::Renderer {

    struct CameraRay {
        CameraRay(const glm::dvec3& orig, const glm::dvec3& direction)
            : origin(orig)
            , dir(direction) {}

        glm::dvec3 origin;
        glm::dvec3 dir;
    };

    class Camera {
    public:
        Camera()
            : m_dist(0.0, 1.0) {}

        void set_fov(double fov_in_degrees) {
            m_fov = 1.0 / std::tan(3.1416 / 180.0 * fov_in_degrees * 0.5);
        }

        void set_pos(glm::dvec3 pos) { m_origin = pos; }

        void set_dir(glm::dvec3 dir) {
            m_forward = dir;

            m_left = glm::abs(dir.z) > 0.99999
                         ? glm::dvec3(0, 1, 0)
                         : glm::normalize(glm::cross(m_forward, glm::dvec3(0, 0, 1)));

            m_up = glm::cross(m_left, m_forward);
        }

        void aim(glm::dvec3 pos) {
            auto dir = glm::normalize(pos - m_origin);
            set_dir(dir);
        }

        CameraRay get_ray(int x, int y, int width, int height) {
            auto aspect = (double)height / (double)width;

            auto tent_x = m_dist(m_re);
            auto tent_y = m_dist(m_re);

            auto xf = 2.0 * (((double)x + tent_x) / (double)width - 0.5);
            auto yf = 2.0 * aspect * (((double)y + tent_y) / (double)height - 0.5);

            auto dir = glm::normalize(m_forward * m_fov + m_left * xf - m_up * yf);

            return CameraRay(m_origin, dir);
        }

    private:
        glm::dvec3 m_origin;

        glm::dvec3 m_forward;
        glm::dvec3 m_left;
        glm::dvec3 m_up;

        double m_fov;

        std::uniform_real_distribution<double> m_dist;
        std::default_random_engine             m_re;
    };

} // namespace Oxy::Renderer
