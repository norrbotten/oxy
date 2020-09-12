#include "color.hpp"

namespace Oxy::Renderer {

    Color::Color()
        : m_r(0)
        , m_g(0)
        , m_b(0) {}

    Color::Color(double a)
        : m_r(a)
        , m_g(a)
        , m_b(a) {}

    Color::Color(double r, double g, double b)
        : m_r(r)
        , m_g(g)
        , m_b(b) {}

    Color& Color::tint(const Color& tint) {
        m_r *= tint.r();
        m_g *= tint.g();
        m_b *= tint.b();
        return *this;
    }

    Color& Color::gamma(double gamma) {
        auto fac = 1.0 / gamma;

        m_r = std::pow(m_r, fac);
        m_g = std::pow(m_g, fac);
        m_b = std::pow(m_b, fac);

        return *this;
    }

} // namespace Oxy::Renderer
