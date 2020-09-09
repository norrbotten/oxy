#pragma once

#include <SFML/Graphics.hpp>

namespace Oxy::Application {

    class PreviewLayer {
    public:
        PreviewLayer(sf::RenderWindow& window, int width, int height);
        ~PreviewLayer();

        sf::Vector2u resolution() const { return m_preview_texture.getSize(); }

        void make_checkerboard();

        void resize(sf::Vector2u size);
        void draw();

        auto* get_mutable_buffer() { return (char*)m_preview_buffer; }

    private:
        sf::RenderWindow& m_window;

        unsigned int m_width = 0, m_height = 0;

        bool        m_preview_dirty;
        sf::Uint8*  m_preview_buffer;
        sf::Texture m_preview_texture;
        sf::Sprite  m_preview_sprite;
    };

} // namespace Oxy::Application