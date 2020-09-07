
#include <SFML/Graphics.hpp>

#include <imgui.h>

#include <imgui-SFML.h>

int main() {
    sf::RenderWindow window(sf::VideoMode(1600, 900), "SFML works!");

    ImGui::SFML::Init(window);

    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    sf::Clock eventloop_clock;

    while (window.isOpen()) {
        sf::Event event;

        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);

            if (event.type == sf::Event::Closed)
                window.close();

            else if (event.type == sf::Event::Resized)
                window.setView(sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height)));
        }

        ImGui::SFML::Update(window, eventloop_clock.restart());
        ImGui::ShowDemoWindow();

        window.clear();

        shape.setPosition(window.getSize().x / 2 - shape.getRadius(),
                          window.getSize().y / 2 - shape.getRadius());
        window.draw(shape);

        ImGui::SFML::Render(window);

        window.display();
    }

    return 0;
}
