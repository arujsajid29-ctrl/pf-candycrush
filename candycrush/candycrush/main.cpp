#include <iostream>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <cstdlib>
#include <ctime>
using namespace std;

const int ROWS = 8;
const int COLS = 8;
int selRow = -1;
int selCol = -1;
int board[ROWS][COLS];
bool marked[ROWS][COLS];

const int CELL = 70;
const int BOARD_X = 20;
const int BOARD_Y = 100;

int findMatches();

bool cellFromClick(int x, int y, int& row, int& col) {
	if (x < BOARD_X || x >= BOARD_X + COLS * CELL) {
		return false;
	}
	if (y < BOARD_Y || y >= BOARD_Y + ROWS * CELL) {
		return false;
	}
	row = (y - BOARD_Y) / CELL;
	col = (x - BOARD_X) / CELL;
	return true;
}

void clearMatches() {
	for (int r = 0; r < ROWS; r++) {
		for (int c = 0; c < COLS; c++) {
			if (marked[r][c]) {
				board[r][c] = 0;
			}
		}
	}
}

void applyGravity() {
	for (int c = 0; c < COLS; c++) {
		int writeRow = ROWS - 1;
		for (int r = ROWS - 1; r >= 0; r--) {
			if (board[r][c] != 0) {
				board[writeRow][c] = board[r][c];
				if (writeRow != r) {
					board[r][c] = 0;
				}
				writeRow--;
			}
		}
	}
}

void refillBoard() {
	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLS; j++) {
			if (board[i][j] == 0) {
				board[i][j] = rand() % 5 + 1;
			}
		}
	}
}

int resolveMatchesAndCascades() {
	int totalMatches = 0;
	int matches = findMatches();
	while (matches > 0) {
		totalMatches += matches;
		clearMatches();
		applyGravity();
		refillBoard();
		matches = findMatches();
	}
	return totalMatches;
}

int resolveMatchesAndCascades();



void handleClick(int x, int y) {
	int row = -1, col = -1;
	if (!cellFromClick(x, y, row, col)) {
		return;
	}
	if (selRow == -1) {
		selRow = row; selCol = col;
		return;
	}
	if (abs(selRow - row) + abs(selCol - col) == 1) {
		int temp = board[selRow][selCol];
		board[selRow][selCol] = board[row][col];
		board[row][col] = temp;
		int totalCleared = resolveMatchesAndCascades();
		if (totalCleared == 0) {
			int back = board[selRow][selCol];
			board[selRow][selCol] = board[row][col];
			board[row][col] = back;
		}
		cout << "swap try | matches found: " << totalCleared << endl;
		selRow = -1;
		selCol = -1;
	}
	else {
		selRow = row; selCol = col;
	}
}

int countHelper_forRows(int row, int col, int counter) {
	for (int j = col; j < col + 3; j++) {
		if (marked[row][j] == false) {
			counter++;
		}
		marked[row][j] = true;
	}

	return counter;
}

int countHelper_forCols(int row, int col, int counter) {
	for (int i = row; i < row + 3; i++) {
		if (marked[i][col] == false) {
			counter++;
		}
		marked[i][col] = true;
	}

	return counter;
}

int findMatches() {
		int counter = 0;
		for (int i = 0; i < ROWS; i++) {// Reset loops
			for (int j = 0; j < COLS; j++) {
				marked[i][j] = false;
			}
		}


	//check matches in row
	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLS - 2; j++) {
			if (board[i][j] == board[i][j + 1] && board[i][j + 1] == board[i][j + 2] && board[i][j] != 0) {
				counter = countHelper_forRows(i, j, counter);
			}
		}
	}
	//check matches in cols
	for(int i = 0; i < ROWS - 2; i++){
		for (int j = 0; j < COLS; j++) {
			if (board[i][j] == board[i + 1][j] && board[i + 1][j] == board[i + 2][j] && board[i][j] != 0) {
				counter = countHelper_forCols(i, j, counter);
			}
		}
	}
	return counter;
}

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
		if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>()) {
			if (mouse->button == sf::Mouse::Button::Left) {
				handleClick(mouse->position.x, mouse->position.y);
			}
		}
	}
}
void updateGame() {

}

void drawGame(sf::RenderWindow & window) {
	window.clear(sf::Color(25, 25, 45));
	sf::CircleShape candy(24.f);
	for (int r = 0; r < ROWS; r++) {
		for (int c = 0; c < COLS; c++) {
			int val = board[r][c];
			if (val != 0) {
				candy.setFillColor(getCandyColor(val));
				candy.setPosition({ BOARD_X + c * CELL + 6.f, BOARD_Y + r * CELL + 6.f });
				window.draw(candy);
			}
		}
	}
	window.display();
}

int main() {
	srand(static_cast<unsigned int>(time(0)));
	loadLevel("level.txt");
	sf::RenderWindow window(sf::VideoMode({ 600, 680 }), "Candy Crush");
	window.setFramerateLimit(60);
	//sf::CircleShape candy(28.f);
	//candy.setFillColor(sf::Color::Red);
	//candy.setPosition({ 20.f, 100.f });
	while (window.isOpen()) {
		handleInput(window);
		updateGame();
		drawGame(window);
	}
	return 0;
}