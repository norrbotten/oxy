#pragma once

#include <SFML/Graphics.hpp>

namespace Oxy::Application {

    class PreviewLayer {
    public:
        PreviewLayer(sf::RenderWindow& window);
        ~PreviewLayer();

        void make_checkerboard();

        void resize(sf::Vector2u size);
        void draw();

    private:
        sf::RenderWindow& m_window;

        unsigned int m_width = 0, m_height = 0;

        bool        m_preview_dirty;
        sf::Uint8*  m_preview_buffer;
        sf::Texture m_preview_texture;
        sf::Sprite  m_preview_sprite;
    };

} // namespace Oxy::Application