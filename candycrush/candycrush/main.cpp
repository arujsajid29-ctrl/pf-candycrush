#include <iostream>
#include <SFML/Graphics.hpp>
#include <fstream>
using namespace std; 

const int ROWS = 8;
const int COLS = 8;
int board[ROWS][COLS];
const float CELL_SIZE = 60.f;
const float OFFSET_X = 60.f;
const float OFFSET_Y = 120.f;

void loadLevel(string filename) {
	ifstream file(filename);
	if (!file.is_open()) {
		cout << "Could not open " << filename << endl;
		return;
	}
	for (int r = 0; r < ROWS; r++) {
		for (int c = 0; c < COLS; c++) {
			file >> board[r][c];
		}
	}
	file.close();
	for (int r = 0; r < ROWS; r++) {
		for (int c = 0; c < COLS; c++) {
			cout << board[r][c] << " ";
		}
		cout << endl;
	}
}
sf::Color getCandyColor(int value) {
	if (value == 1) return sf::Color::Red;
	if (value == 2) return sf::Color::Green;
	if (value == 3) return sf::Color::Blue;
	if (value == 4) return sf::Color::Yellow;
	if (value == 5) return sf::Color::Magenta;
	return sf::Color::Transparent;
}

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
bool marked[ROWS][COLS];

int findMatches() {
	int counter = 0;
	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLS; j++) {
			marked[i][j] = false;
			if (board[i][j] == board[i][j + 1] == board[i][j + 2] || board[i][j] == board[i + 1][j] == board[i + 2][j]) {
				marked[i][j] = true;
				counter++;
			}
		}
	}
	return counter; 
}

void drawGame(sf::RenderWindow& window) {
	window.clear(sf::Color(25, 25, 45));
	sf::CircleShape candy(24.f);
	for (int r = 0; r < ROWS; r++) {
		for (int c = 0; c < COLS; c++) {
			int val = board[r][c];
			if (val != 0) {
				candy.setFillColor(getCandyColor(val));
				candy.setPosition({ OFFSET_X + c * CELL_SIZE + 6.f, OFFSET_Y + r * CELL_SIZE + 6.f });
				window.draw(candy);
			}
		}
	}
	window.display();
}
int main() {
	loadLevel("level.txt");
	sf::RenderWindow window(sf::VideoMode({ 600, 680 }), "Candy Crush");
	window.setFramerateLimit(60);
	while (window.isOpen()) {
		handleInput(window);
		updateGame();
		drawGame(window);
	}
	return 0;
}