#include <iostream>
#include<string>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include<utility>
using namespace std;

sf::Font font;
const int ROWS = 8;
const int COLS = 8;
int selRow = -1;
int selCol = -1;
const float CANDY_OFFSET = 11.f;
int board[ROWS][COLS];
bool marked[ROWS][COLS];

const float CELL = 70.f;
const float BOARD_X = 20.f;
const float BOARD_Y = 100.f;

const int POINTS_PER_CANDY = 10;
int score = 0;
int movesleft = 0, targetScore = 0;
bool isWon = false;
bool Gameover = false;

int resolveMatchesAndCascades();

bool trySwap(int r1, int c1, int r2, int c2)
{
	swap(board[r1][c1], board[r2][c2]);
	int cleared = resolveMatchesAndCascades();
	if (cleared == 0)
	{
		swap(board[r2][c2], board[r1][c1]);
		return false;
	}
	else
	{
		score += cleared * POINTS_PER_CANDY;
		movesleft--;
		return true;
	}
}

bool loadLevel(string filename)
{
	score = 0;
	int fileRows, fileCols;
	ifstream file(filename);

	if (!file.is_open())
	{
		return false;
	}
	file >> fileRows >> fileCols;
	file >> movesleft >> targetScore;
	for (int i = 0;i < fileRows;i++)
	{
		for (int j = 0;j < fileCols;j++)
		{
			file >> board[i][j];
		}
	}
	for (int r = 0; r < ROWS; r++) {
		for (int c = 0; c < COLS; c++) {
			cout << board[r][c] << " ";
		}
		cout << endl;
	}
	file.close();
	return true;
}

int findMatches();

bool cellFromClick(int x, int y, int& row, int& col) {
	if (x < BOARD_X || x >= BOARD_X + COLS * CELL) {
		return false;
	}
	if (y < BOARD_Y || y >= BOARD_Y + ROWS * CELL) {
		return false;
	}
	row = static_cast<int>((y - BOARD_Y) / CELL);
	col = static_cast<int>((x - BOARD_X) / CELL);
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
				board[i][j] = rand() % 6 + 1;
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

void chcekGameStatus()
{
	if (score >= targetScore)
	{
		isWon = true;
	}
	else if (movesleft <= 0)
	{
		Gameover = true;
	}
}

void handleClick(int x, int y) {
	if (isWon || Gameover)
	{
		return;
	}
	int row = -1, col = -1;
	if (movesleft <= 0) {
		return;
	}
	if (!cellFromClick(x, y, row, col)) {
		return;
	}
	if (selRow == -1) {
		selRow = row; selCol = col;
		return;
	}
	if (abs(selRow - row) + abs(selCol - col) == 1) {
		trySwap(selRow, selCol, row, col);
		selRow = -1;
		selCol = -1;
		chcekGameStatus();
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

sf::Color getCandyColor(int value) {
	if (value == 1) return sf::Color::Red;
	if (value == 2) return sf::Color::Green;
	if (value == 3) return sf::Color::Blue;
	if (value == 4) return sf::Color::Yellow;
	if (value == 5) return sf::Color::Magenta;
	if (value == 6) return sf::Color(170, 90, 255);
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

void drawSelectionRing(sf::RenderWindow& window) {
	if (selRow == -1) {
		return;
	}
	sf::CircleShape ring(24.f);
	ring.setFillColor(sf::Color::Transparent);
	ring.setOutlineThickness(4.f);
	ring.setOutlineColor(sf::Color::White);
	ring.setPosition({ BOARD_X + selCol * CELL + CANDY_OFFSET, BOARD_Y + selRow * CELL + CANDY_OFFSET });
	window.draw(ring);
}


void drawGame(sf::RenderWindow & window) {
	window.clear(sf::Color(25, 25, 45));
	sf::CircleShape candy(24.f);
	sf::RectangleShape cell({ CELL * 1.f, CELL * 1.f });
	cell.setFillColor(sf::Color(15, 15, 30));
	cell.setOutlineThickness(1.f);
	cell.setOutlineColor(sf::Color(60, 60, 90));
	for (int r = 0; r < ROWS; r++) {
		for (int c = 0; c < COLS; c++) {
			int val = board[r][c];
			cell.setPosition({ BOARD_X + c * CELL, BOARD_Y + r * CELL });
			window.draw(cell);
			if (val != 0) {
				candy.setFillColor(getCandyColor(val));
				candy.setPosition({ BOARD_X + c * CELL + CANDY_OFFSET, BOARD_Y + r * CELL + CANDY_OFFSET});
				window.draw(candy);
			}
		}
	}
	drawSelectionRing(window);
	sf::Text hudText(font);
	hudText.setCharacterSize(24);
	hudText.setFillColor(sf::Color::White);
	hudText.setPosition({ 20.f, 20.f });
	if (isWon) {
		hudText.setString("You Win! Score: " + to_string(score));
	}
	else if (Gameover) {
		hudText.setString("Game Over! Out of Moves.");
	}
	else {
		hudText.setString("Score: " + to_string(score) + "       Moves: " + to_string(movesleft) + "       Target: " + to_string(targetScore));
	}
	window.draw(hudText);
	window.display();
}

int main() {
	
	//Firstly Loading the levelLoad
	if (!loadLevel("level.txt"))
	{
		//cout << "Error in loadLevel file Oops!" << endl;
		return -1;
	}
	srand(static_cast<unsigned int>(time(0)));
	if (!font.openFromFile("C:/Windows/Fonts/arial.ttf"))
	{
		//cout << "Font is not loading" << endl;
	}
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