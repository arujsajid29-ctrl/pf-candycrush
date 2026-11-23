#include <SFML/Graphics.hpp>

int main()
{
    sf::RenderWindow window(sf::VideoMode({ 600, 680 }), "Candy Crush");
    window.setFramerateLimit(60);

    sf::CircleShape candy(28.f);
    candy.setFillColor(sf::Color::Red);
    candy.setPosition({ 20.f, 100.f });

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear(sf::Color(25, 25, 45));
        window.draw(candy);
        window.display();
    }
    return 0;
}