#pragma once

#include <cmath>

namespace Oxy::Renderer {

    class Color final {
    public:
        Color();
        Color(double r, double g, double b);

        const auto& r() const { return m_r; }
        const auto& g() const { return m_g; }
        const auto& b() const { return m_b; }

        Color& tint(const Color& tint);
        Color& gamma(double gamma = 2.2);

        void to_chars(char& r, char& g, char& b) const {
            auto convert = [](double val) -> char {
                return val < 0.0 ? (char)0 : (val > 1.0 ? (char)255 : (char)(val * 255.0));
            };

            r = convert(m_r);
            g = convert(m_g);
            b = convert(m_b);
        }

    private:
        double m_r, m_g, m_b;
    };

} // namespace Oxy::Renderer
