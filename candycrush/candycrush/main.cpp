#include <iostream>
#include <SFML/Graphics.hpp>
using namespace std; 

void handleInput(sf::RenderWindow& window) {
	while (true) {
		auto event = window.pollEvent();
		if (!event.has_value()) {
			break;
		}
		if (event->is<sf::Event::Closed>()) {
			window.close();
		}
	}
}
void updateGame() {

}
void drawGame(sf::RenderWindow& window, sf::CircleShape& candy) {
	window.clear(sf::Color(25, 25, 45));
	window.draw(candy);
	window.display();
}
int main() {
	sf::RenderWindow window(sf::VideoMode({ 600, 680 }), "Candy Crush");
	window.setFramerateLimit(60);
	sf::CircleShape candy(28.f);
	candy.setFillColor(sf::Color::Red);
	candy.setPosition({ 20.f, 100.f });
	while (window.isOpen()) {
		handleInput(window);
		updateGame();
		drawGame(window, candy);
	}
	return 0;
}