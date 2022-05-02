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

struct ShipInfo {
		std::string name;
		int count;
		int length;
};

// Строка - название корабля,
// первое число пары - количество кораблей,
// Второе число пары - длина корабля
std::vector<ShipInfo> SHIPS = {
	{"Четырёхпалубник", 1, 4},
	{"Трёхпалубник", 2, 3},
	{"Двухпалубник", 3, 2},
	{"Однопалубник", 4, 1}
};

class Map {
	public:

		Map() : data() {
			this->clear();
		}

		Map(bool map[MAPSIZE][MAPSIZE]) {
			for (int i = 0; i < MAPSIZE; ++i)
				for (int j = 0; j < MAPSIZE; ++j)
					data[i][j] = map[i][j];
		}

		bool get(int col, int row) const {
			if (col < 0 || row < 0 || col >= MAPSIZE || row >= MAPSIZE)
				return false;
			return data[row][col];
		}

		void generateRandomMap() {
			std::random_device dev;
			std::mt19937 rng(dev());
			std::uniform_int_distribution<std::mt19937::result_type> randomMapNumber(0, MAPSIZE-1);
			std::uniform_int_distribution<std::mt19937::result_type> randomDirection(0, 3);
			for (auto it = SHIPS.begin(); it != SHIPS.end(); ++it)
				for (int shipsCount = 0; shipsCount < it->count; ++shipsCount)
					while (!this->setShipIfCanPlace(randomMapNumber(rng), randomMapNumber(rng), ShipDirection(randomDirection(rng)), it->length));
		}

		friend void operator<<(std::ostream &stream, const Map &m) {
			stream << "   "; // Красивый отступ

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
			stream << "\n";
			// Выводим карту
			for (int i = 0; i < MAPSIZE; ++i) {
				stream << i+1 << (i == 9 ? "" : " ") <<"|";
				for (int j = 0; j < MAPSIZE; ++j){
					stream << (m.get(i, j) ? "#" : " ") << "|";
				}
				stream << "\n";
			}
		}

		void print() const {
			cout << (*this);
		}

		void clear() {
			for (int i = 0; i < MAPSIZE; ++i)
				for (int j = 0; j < MAPSIZE; ++j)
					this->data[i][j] = false;
		}


	private:
		bool canPlaceShipInCell(int col, int row) {
			for (int i = -1; i < 2; ++i) {
				for (int j = -1; j < 2; ++ j) {
					if (row+i < 0 || row+i >= MAPSIZE || col+j < 0 || col+j >= MAPSIZE)
						continue;
					if (this->data[row+i][col+j])
						return false;
				}
			}
			return true;
		}

		bool setShipIfCanPlace(int col, int row, ShipDirection direction, int length) {
			if (col < 0 || col >= MAPSIZE || row < 0 || row >= MAPSIZE)
				return false;

			switch (direction) {
				case ShipDirection::LEFT:
					if (col - length < 0)
						return false;

					for (int i = 0; i < length; ++i)
						if (!this->canPlaceShipInCell(col-i, row))
							return false;

					for (int i = 0; i < length; ++i)
						this->data[row][col-i] = true;
					break;

				case ShipDirection::UP:
					if (row - length < 0)
						return false;

					for (int i = 0; i < length; ++i)
						if (!this->canPlaceShipInCell(col, row-i))
							return false;

					for (int i = 0; i < length; ++i)
						this->data[row-i][col] = true;
					break;

				case ShipDirection::RIGHT:
					if (col + length >= MAPSIZE)
						return false;

					for (int i = 0; i < length; ++i)
						if (!this->canPlaceShipInCell(col+i, row))
							return false;

					for (int i = 0; i < length; ++i)
						this->data[row][col+i] = true;
					break;

				case ShipDirection::DOWN:
					if (row + length >= MAPSIZE)
						return false;

					for (int i = 0; i < length; ++i)
						if (!this->canPlaceShipInCell(col, row+i))
							return false;

					for (int i = 0; i < length; ++i)
						this->data[row+i][col] = true;
					break;

				default:
					return false;
			}
			return true;
		}


	private:
		bool data[MAPSIZE][MAPSIZE];
};




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

	bool botMap[MAPSIZE][MAPSIZE];


	return 0;
}
