#include <iostream>
#include <SFML/Graphics.hpp>
using namespace std; 

<<<<<<< Updated upstream
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
=======
const int rows = 8;
const int cols = 8;

bool marked[rows][cols];
int board[rows][cols];

int findMatches() {
	int counter = 0;
	for (int i = 0; i < rows; i++) {// Reset loops
		for (int j = 0; j < cols; j++) {
			marked[i][j] = false;
			if (board[i][j] == board[i][j + 1] == board[i][j + 2] || board[i][j] == board[i + 1][j] == board[i + 2][j]) {
				marked = true;
				counter++;
			}
		}
	}

	return counter;
>>>>>>> Stashed changes
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