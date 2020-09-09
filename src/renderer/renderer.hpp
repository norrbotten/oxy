#pragma once

#include "renderer/utils/sample_film.hpp"

namespace Oxy::Renderer {

    class OxyRenderer final {
    public:
        OxyRenderer();

        auto get_render_width() const { return m_film.width(); }
        auto get_render_height() const { return m_film.height(); }

        void set_render_resolution(int width, int height) { m_film.resize(width, height); }

        void render_fractal();

        const auto& film() { return m_film; }

    private:
        SampleFilm m_film;
    };

} // namespace Oxy::Renderer
