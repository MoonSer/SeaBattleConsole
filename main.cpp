#include <iostream>
#include <random>
#include <map>
#include <stdio.h>
#include <fcntl.h>
using namespace std;

#define MAPSIZE 10

enum ShipDirection {
	LEFT, UP, RIGHT, DOWN
};

// Строка - название корабля,
// первое число пары - количество кораблей,
// Второе число пары - длина корабля
std::map<std::string, std::pair<int, int>> SHIPS = {
	{"Четырёхпалубник", {1, 4}},
	{"Трёхпалубник", {2, 3}},
	{"Двухпалубник", {3, 2}},
	{"Однопалубник", {4, 1}}
};

bool checkDot(bool map[MAPSIZE][MAPSIZE], int col, int row) {
	for (int i = -1; i < 2; ++i) {
		for (int j = -1; j < 2; ++ j) {
			if (row+i < 0 || row+i >= MAPSIZE || col+j < 0 || col+j >= MAPSIZE)
				continue;
			if (map[row+i][col+j])
				return false;
		}
	}
	return true;
}

bool checkShipAndSet(bool map[MAPSIZE][MAPSIZE], int col, int row, int direction, int length) {
	if (col < 0 || col >= MAPSIZE || row < 0 || row >= MAPSIZE)
		return false;
	switch (direction) {
		case ShipDirection::LEFT:
			if (col - length < 0)
				return false;
			for (int i = 0; i < length; ++i)
				if (!checkDot(map, col-i, row))
					return false;
			for (int i = 0; i < length; ++i)
				map[row][col-i] = true;
			break;
		case ShipDirection::UP:
			if (row - length < 0)
				return false;
			for (int i = 0; i < length; ++i)
				if (!checkDot(map, col, row-i))
					return false;
			for (int i = 0; i < length; ++i)
				map[row-i][col] = true;
			break;
		case ShipDirection::RIGHT:
			if (col + length >= MAPSIZE)
				return false;
			for (int i = 0; i < length; ++i)
				if (!checkDot(map, col+i, row))
					return false;
			for (int i = 0; i < length; ++i)
				map[row][col+i] = true;
			break;
		case ShipDirection::DOWN:
			if (row + length >= MAPSIZE)
				return false;
			for (int i = 0; i < length; ++i)
				if (!checkDot(map, col, row+i))
					return false;
			for (int i = 0; i < length; ++i)
				map[row+i][col] = true;
			break;
		default:
			return false;
	}
	return true;
}

void generateRandomMap(bool map[MAPSIZE][MAPSIZE]) {
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> randomMapNumber(0, MAPSIZE-1);
	std::uniform_int_distribution<std::mt19937::result_type> randomDirection(0, 3);
	for (auto it = SHIPS.begin(); it != SHIPS.end(); ++it)
		for (int shipsCount = 0; shipsCount < (*it).second.first; ++shipsCount)
			while (!checkShipAndSet(map, randomMapNumber(rng), randomMapNumber(rng), randomDirection(rng), (*it).second.second));
}

void printMap(bool map[MAPSIZE][MAPSIZE]) {
	cout << "   "; // Красивый отступ

	// Штука в том, что символ юникод не влазит в один char, так что храним в массиве char'ов
	// При необходимости итерирования к следующей буковке - увеличиваем элемент с первым индексом

	char alphabetLetter[8]; // Тут будем хранить текуую букву

	// проблема в том, что "\u0430" - константный массив. А нам нужно менять первый элемент. Поэтому копируем в изменяемый массив:
	for (int i = 0; i < 8; ++i)
		alphabetLetter[i] = "\u0430"[i]; // Символ а в юникоде
	alphabetLetter[1] += -1; // Из-за того, что в циклы мы сперва меняем букву приходится начинать с -1 позиции

	// Выводим наши буквы на экран
	for (int i = 0; i < MAPSIZE; ++i) {
		// Пропускаем букву й
		if (i == MAPSIZE-1)
			alphabetLetter[1] += 1;

		// Меняем символ на последующий
		alphabetLetter[1] += 1;

		cout << alphabetLetter << " ";
	}
	cout << "\n";
	// Выводим карту
	for (int i = 0; i < MAPSIZE; ++i) {
		cout << i+1 << (i == 9 ? "" : " ") <<"|";
		for (int j = 0; j < MAPSIZE; ++j){
			cout << (map[i][j] ? "#" : " ") << "|";
		}
		cout << "\n";
	}
}

void clear(bool map[MAPSIZE][MAPSIZE]) {
	for (int i = 0; i < MAPSIZE; ++i) {
		for (int j = 0; j < MAPSIZE; ++j) {
			map[i][j] = false;
		}
	}
}

int main() {

	cout << "                     _______. _______      ___       " << "\n"
		 << "                    /       ||   ____|    /   \\     " << "\n"
		 << "                   |   (----`|  |__      /  ^  \\    " << "\n"
		 << "                    \\   \\    |   __|    /  /_\\  \\" << "\n"
		 << "                .----)   |   |  |____  /  _____  \\  " << "\n"
		 << "                |_______/    |_______|/__/     \\__\\" << "\n";

	cout << "  .______        ___      .___________..___________. __       _______ " << "\n"
		 << "  |   _  \\      /   \\     |           ||           ||  |     |   ____|" << "\n"
		 << "  |  |_)  |    /  ^  \\    `---|  |----``---|  |----`|  |     |  |__   " << "\n"
		 << "  |   _  <    /  /_\\  \\       |  |         |  |     |  |     |   __|  " << "\n"
		 << "  |  |_)  |  /  _____  \\      |  |         |  |     |  `----.|  |____  " << "\n"
		 << "  |______/  /__/     \\__\\     |__|         |__|     |_______||_______|" << "\n";

	bool playerMap[MAPSIZE][MAPSIZE];
	clear(playerMap);

	bool botMap[MAPSIZE][MAPSIZE];
	clear(botMap);
	generateRandomMap(botMap);

	printMap(botMap);



	return 0;
}

/*
 * #include <iostream>
#include <random>
#include <map>
#include <stdio.h>
#include <fcntl.h>
using namespace std;

#define MAPSIZE 10

enum ShipDirection {
	LEFT, UP, RIGHT, DOWN
};

class MapRow {
	public:
		virtual bool isValid() const {
			return true;
		}
		bool operator[](int x) {
			if (!this->isValid() || x < 0 || x >= MAPSIZE)
				return false;
			return this->data[x];
		}
};

class InvalidRow : public MapRow {
	public:
		bool isValid() const override {
			return false;
		}
};

class Map {
	public:

		Map() : data() {}
		Map(bool map[MAPSIZE][MAPSIZE]) {
			for (int i = 0; i < MAPSIZE; ++i)
		}

		bool get(int col, int row) const {
			if (col < 0 || row < 0 || col >= MAPSIZE || row >= MAPSIZE)
				return false;
			return data[row][col];
		}

		MapRow operator[](int row) const{
			if (row < 0 || row >= MAPSIZE)
				return InvalidRow();
			return data[row];
		}

	private:
		MapRow data[MAPSIZE][MAPSIZE];
};

// Строка - название корабля,
// первое число пары - количество кораблей,
// Второе число пары - длина корабля
std::map<std::string, std::pair<int, int>> SHIPS = {
	{"Четырёхпалубник", {1, 4}},
	{"Трёхпалубник", {2, 3}},
	{"Двухпалубник", {3, 2}},
	{"Однопалубник", {4, 1}}
};

bool checkDot(bool map[MAPSIZE][MAPSIZE], int col, int row) {
	for (int i = -1; i < 2; ++i) {
		for (int j = -1; j < 2; ++ j) {
			if (row+i < 0 || row+i >= MAPSIZE || col+j < 0 || col+j >= MAPSIZE)
				continue;
			if (map[row+i][col+j])
				return false;
		}
	}
	return true;
}

bool checkShipAndSet(bool map[MAPSIZE][MAPSIZE], int col, int row, int direction, int length) {
	if (col < 0 || col >= MAPSIZE || row < 0 || row >= MAPSIZE)
		return false;
	switch (direction) {
		case ShipDirection::LEFT:
			if (col - length < 0)
				return false;
			for (int i = 0; i < length; ++i)
				if (!checkDot(map, col-i, row))
					return false;
			for (int i = 0; i < length; ++i)
				map[row][col-i] = true;
			break;
		case ShipDirection::UP:
			if (row - length < 0)
				return false;
			for (int i = 0; i < length; ++i)
				if (!checkDot(map, col, row-i))
					return false;
			for (int i = 0; i < length; ++i)
				map[row-i][col] = true;
			break;
		case ShipDirection::RIGHT:
			if (col + length >= MAPSIZE)
				return false;
			for (int i = 0; i < length; ++i)
				if (!checkDot(map, col+i, row))
					return false;
			for (int i = 0; i < length; ++i)
				map[row][col+i] = true;
			break;
		case ShipDirection::DOWN:
			if (row + length >= MAPSIZE)
				return false;
			for (int i = 0; i < length; ++i)
				if (!checkDot(map, col, row+i))
					return false;
			for (int i = 0; i < length; ++i)
				map[row+i][col] = true;
			break;
		default:
			return false;cout << InvalidRow().isValid() << "\n";
	}
	return true;
}

void generateRandomMap(bool map[MAPSIZE][MAPSIZE]) {
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> randomMapNumber(0, MAPSIZE-1);
	std::uniform_int_distribution<std::mt19937::result_type> randomDirection(0, 3);
	for (auto it = SHIPS.begin(); it != SHIPS.end(); ++it)
		for (int shipsCount = 0; shipsCount < (*it).second.first; ++shipsCount)
			while (!checkShipAndSet(map, randomMapNumber(rng), randomMapNumber(rng), randomDirection(rng), (*it).second.second));
}

void printMap(bool map[MAPSIZE][MAPSIZE]) {
	cout << "   "; // Красивый отступ

	// Штука в том, что символ юникод не влазит в один char, так что храним в массиве char'ов
	// При необходимости итерирования к следующей буковке - увеличиваем элемент с первым индексом

	char alphabetLetter[8]; // Тут будем хранить текуую букву

	// проблема в том, что "\u0430" - константный массив. А нам нужно менять первый элемент. Поэтому копируем в изменяемый массив:
	for (int i = 0; i < 8; ++i)
		alphabetLetter[i] = "\u0430"[i]; // Символ а в юникоде
	alphabetLetter[1] += -1; // Из-за того, что в циклы мы сперва меняем букву приходится начинать с -1 позиции

	// Выводим наши буквы на экран
	for (int i = 0; i < MAPSIZE; ++i) {
		// Пропускаем букву й
		if (i == MAPSIZE-1)
			alphabetLetter[1] += 1;

		// Меняем символ на последующий
		alphabetLetter[1] += 1;

		cout << alphabetLetter << " ";
	}
	cout << "\n";
	// Выводим карту
	for (int i = 0; i < MAPSIZE; ++i) {
		cout << i+1 << (i == 9 ? "" : " ") <<"|";
		for (int j = 0; j < MAPSIZE; ++j){
			cout << (map[i][j] ? "#" : " ") << "|";
		}
		cout << "\n";
	}
}

void clear(bool map[MAPSIZE][MAPSIZE]) {
	for (int i = 0; i < MAPSIZE; ++i) {
		for (int j = 0; j < MAPSIZE; ++j) {
			map[i][j] = false;
		}
	}
}

int main() {

	cout << "                     _______. _______      ___       " << "\n"
		 << "                    /       ||   ____|    /   \\     " << "\n"
		 << "                   |   (----`|  |__      /  ^  \\    " << "\n"
		 << "                    \\   \\    |   __|    /  /_\\  \\" << "\n"
		 << "                .----)   |   |  |____  /  _____  \\  " << "\n"
		 << "                |_______/    |_______|/__/     \\__\\" << "\n";

	cout << "  .______        ___      .___________..___________. __       _______ " << "\n"
		 << "  |   _  \\      /   \\     |           ||           ||  |     |   ____|" << "\n"
		 << "  |  |_)  |    /  ^  \\    `---|  |----``---|  |----`|  |     |  |__   " << "\n"
		 << "  |   _  <    /  /_\\  \\       |  |         |  |     |  |     |   __|  " << "\n"
		 << "  |  |_)  |  /  _____  \\      |  |         |  |     |  `----.|  |____  " << "\n"
		 << "  |______/  /__/     \\__\\     |__|         |__|     |_______||_______|" << "\n";

	bool playerMap[MAPSIZE][MAPSIZE];
	clear(playerMap);

	bool botMap[MAPSIZE][MAPSIZE];
	clear(botMap);
	generateRandomMap(botMap);

	printMap(botMap);

	cout << InvalidRow().isValid() << "\n";

	MapRow r = InvalidRow();
	cout << r.isValid() << "\n";

	return 0;
}
*/
