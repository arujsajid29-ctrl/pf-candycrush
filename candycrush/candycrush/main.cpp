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
bool outOfMoves = false;
bool hasHint = false;
int hintRow = -1;
int hintCol = -1;
bool isWon = false;
bool Gameover = false;

int resolveMatchesAndCascades();
void checkGameStatus();
void clearSaveFile();

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
		checkGameStatus();
		if (movesleft <= 0) {
			movesleft = 0;
			if (score < targetScore) {
				outOfMoves = true;
			}
		}
		return true;
	}
}

bool loadLevel(string filename)
{
	score = 0;
	outOfMoves = false;
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

bool findHintMove(int& r1, int& c1, int& r2, int& c2) {
	for (int r = 0; r < ROWS; r++) {
		for (int c = 0; c < COLS; c++) {
			if (c + 1 < COLS) {
				swap(board[r][c], board[r][c + 1]);
				bool matched = (findMatches() > 0);
				swap(board[r][c], board[r][c + 1]);
				if (matched) {
					r1 = r; c1 = c; r2 = r; c2 = c + 1;
					return true;
				}
			}
			if (r + 1 < ROWS) {
				swap(board[r][c], board[r + 1][c]);
				bool matched = (findMatches() > 0);
				swap(board[r][c], board[r + 1][c]);
				if (matched) {
					r1 = r; c1 = c; r2 = r + 1; c2 = c;
					return true;
				}
			}
		}
	}
	return false;
}

void shuffleBoard() {
	for (int r = 0; r < ROWS; r++) {
		for (int c = 0; c < COLS; c++) {
			int randR = rand() % ROWS;
			int randC = rand() % COLS;
			swap(board[r][c], board[randR][randC]);
		}
	}
	selRow = -1;
	selCol = -1; 
	hasHint = false;
	hintRow = -1;
	hintCol = -1;
}

void checkGameStatus()
{
	if (score >= targetScore)
	{
		isWon = true;
		clearSaveFile();
	}
	else if (movesleft <= 0)
	{
		movesleft = 0;
		Gameover = true;
		outOfMoves = true;
		clearSaveFile();
	}
}

void handleClick(int x, int y) {
	if (isWon || Gameover || movesleft <= 0 || outOfMoves) {
		return;
	}
	int row = -1, col = -1;
	hasHint = false;
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

void clearSaveFile() {
	ofstream out("savegame.txt", ios::trunc);
	out.close();
}

bool saveGameToFile() {
	ofstream out("savegame.txt");
	if (!out.is_open()) {
		return false;
	}
	out << score << " " << movesleft << " " << targetScore << "\n";
	for (int r = 0; r < ROWS; r++) {
		for (int c = 0; c < COLS; c++) {
			out << board[r][c];
			if (c < COLS - 1) {
				out << " ";
			}
		}
		out << "\n";
	}
	out.close();
	return true;
}

bool loadSavedGame() {
	ifstream in("savegame.txt");
	if (!in.is_open()) {
		return false;
	}
	int savedScore = 0, savedMoves = 0, savedTarget = 0;
	if (!(in >> savedScore >> savedMoves >> savedTarget)) {
		in.close();
		return false;
	}
	int tempBoard[ROWS][COLS];
	for (int r = 0; r < ROWS; r++) {
		for (int c = 0; c < COLS; c++) {
			if (!(in >> tempBoard[r][c])) {
				in.close();
				return false;
			}
		}
	}
	in.close();
	for (int r = 0; r < ROWS; r++) {
		for (int c = 0; c < COLS; c++) {
			board[r][c] = tempBoard[r][c];
		}
	}
	score = savedScore; 
	movesleft = savedMoves;
	targetScore = savedTarget;
	selRow = -1;
	selCol = -1;
	hasHint = false;
	hintRow = -1; 
	hintCol = -1;
	isWon = false;
	Gameover = false;
	outOfMoves = false;
	return true;
}

bool inMenu = true;
bool noSavedGame = false;

void handleInput(sf::RenderWindow& window) {
	while (true) {
		auto event = window.pollEvent();
		if (!event.has_value()) {
			break;
		}
		if (event->is<sf::Event::Closed>()) {
			window.close();
		}
		if (!inMenu) {
			if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>()) {
				if (mouse->button == sf::Mouse::Button::Left) {
					handleClick(mouse->position.x, mouse->position.y);
				}
			}
		}
		if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
			if (inMenu) {
				if (key->code == sf::Keyboard::Key::Enter) {
					loadLevel("level.txt");
					noSavedGame = false;
					inMenu = false;
				}
				else if (key->code == sf::Keyboard::Key::C) {
					if (loadSavedGame()) {
						noSavedGame = false;
						inMenu = false;
					}
					else {
						noSavedGame = true;
					}
				}
				continue;
			}

			if (key->code == sf::Keyboard::Key::Escape && !isWon && !Gameover && !outOfMoves) {
				saveGameToFile();
				selRow = -1;
				selCol = -1;
				hasHint = false;
				hintRow = -1;
				hintCol = -1;
				inMenu = true;
				continue;
			}
			if (key->code == sf::Keyboard::Key::H && !outOfMoves && movesleft > 0) {
				int r1 = -1, c1 = -1, r2 = -1, c2 = -1;
				if (findHintMove(r1, c1, r2, c2)) {
					hasHint = true;
					hintRow = r1;
					hintCol = c1;
				}
			}
			else if (key->code == sf::Keyboard::Key::S && !outOfMoves && movesleft > 0) {
				shuffleBoard();
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
	if (inMenu) {
		sf::Text titleText(font);
		titleText.setCharacterSize(32);
		titleText.setFillColor(sf::Color::White);
		titleText.setPosition({ 120.f, 180.f });
		titleText.setString("Candy Crush");
		window.draw(titleText);

		sf::Text playText(font);
		playText.setCharacterSize(22);
		playText.setFillColor(sf::Color::White);
		playText.setPosition({ 120.f, 250.f });
		playText.setString("Press ENTER to Play");
		window.draw(playText);

		sf::Text continueText(font);
		continueText.setCharacterSize(22);
		continueText.setFillColor(sf::Color::Cyan);
		continueText.setPosition({ 120.f, 290.f });
		continueText.setString("Press C to Continue");
		window.draw(continueText);

		if (noSavedGame) {
			sf::Text noSaveText(font);
			noSaveText.setCharacterSize(22);
			noSaveText.setFillColor(sf::Color(255, 90, 90));
			noSaveText.setPosition({ 120.f, 340.f });
			noSaveText.setString("No saved game");
			window.draw(noSaveText);
		}

		window.display();
		return;
	}
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
	if (hasHint && hintRow != -1 && hintCol != -1) {
		sf::CircleShape hintRing(26.f);
		hintRing.setFillColor(sf::Color::Transparent);
		hintRing.setOutlineThickness(4.f);
		hintRing.setOutlineColor(sf::Color::Cyan);
		hintRing.setPosition({ BOARD_X + hintCol * CELL + CANDY_OFFSET - 2.f, BOARD_Y + hintRow * CELL + CANDY_OFFSET - 2.f });
		window.draw(hintRing);
	}
	sf::Text hudText(font);
	hudText.setCharacterSize(24);
	hudText.setFillColor(sf::Color::White);
	hudText.setPosition({ 20.f, 20.f });
	if (isWon) {
		hudText.setString("You Win! Score: " + to_string(score));
	}
	else if (Gameover || outOfMoves) {
		hudText.setString("Game Over! Out of Moves.");
	}
	else {
		hudText.setString("Score: " + to_string(score) + "\tMoves: " + to_string(movesleft) + "\tTarget: " + to_string(targetScore));
	}
	window.draw(hudText);
	if (outOfMoves) {
		sf::Text loseText(font);
		loseText.setCharacterSize(22);
		loseText.setFillColor(sf::Color(255, 90, 90));
		loseText.setPosition({ 20.f, 55.f });
		loseText.setString("Out of Moves! Target not reached.");
		window.draw(loseText);
	}
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