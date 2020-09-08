#include "app/preview_layer.hpp"

#include <iostream>

namespace Oxy::Application {

    PreviewLayer::PreviewLayer(sf::RenderWindow& window)
        : m_window(window)
        , m_preview_dirty(true)
        , m_preview_buffer(nullptr) {

        resize(sf::Vector2u(512, 512));
        make_checkerboard();
    }

    PreviewLayer::~PreviewLayer() {
        if (m_preview_buffer != nullptr)
            delete m_preview_buffer;
    }

    void PreviewLayer::make_checkerboard() {
        for (unsigned int y = 0; y < m_height; y++) {
            for (unsigned int x = 0; x < m_width; x++) {
                auto xx = x / 64;
                auto yy = y / 64;

                auto col = ((xx + yy) % 2 == 0) ? 0x10 : 0x15;

                m_preview_buffer[4 * (x + y * m_width) + 0] = col;
                m_preview_buffer[4 * (x + y * m_width) + 1] = col;
                m_preview_buffer[4 * (x + y * m_width) + 2] = col;
                m_preview_buffer[4 * (x + y * m_width) + 3] = 0xFF;
            }
        }

        m_preview_dirty = true;
    }

    void PreviewLayer::resize(sf::Vector2u size) {
        if (m_preview_buffer != nullptr)
            delete m_preview_buffer;

        m_width  = size.x;
        m_height = size.y;

        m_preview_buffer = new sf::Uint8[size.x * size.y * 4];
        m_preview_texture.create(size.x, size.y);

        m_preview_dirty = true;

        make_checkerboard();
    }

    void PreviewLayer::draw() {
        if (m_preview_dirty) {
            m_preview_dirty = false;

            m_preview_texture.update(m_preview_buffer);
            m_preview_sprite.setTexture(m_preview_texture, true);
        }

        // letterbox the sprite
        auto [window_w, window_h] = m_window.getSize();
        auto [sprite_w, sprite_h] = m_preview_texture.getSize();

        auto width_ratio  = (double)window_w / (double)sprite_w;
        auto height_ratio = (double)window_h / (double)sprite_h;

        auto best_ratio = std::min(width_ratio, height_ratio);

        auto new_width  = sprite_w * best_ratio;
        auto new_height = sprite_h * best_ratio;

        auto x_diff = window_w - new_width;
        auto y_diff = window_h - new_height;

        m_preview_sprite.setScale(sf::Vector2f(best_ratio, best_ratio));
        m_preview_sprite.setPosition(sf::Vector2f(x_diff / 2, y_diff / 2));

        m_window.draw(m_preview_sprite);
    }

} // namespace Oxy::Application
