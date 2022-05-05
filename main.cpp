#include <iostream>
#include <random>
#include <vector>
#include <filesystem>
#include <fstream>
#include <string>
#include <cstring>
using namespace std;

#define MAPSIZE 10


struct ShipInfo {
		std::string name;
		int count;
		int length;
};


class Map {

		enum ShipDirection {
			LEFT, UP, RIGHT, DOWN
		};

		enum RecognizeError {
			NoError, IncorrectDirection, IncorrectCell, CellAlreadyUsed
		};

	public:

		Map() : data() {
			this->clear();
		}

		Map(bool map[MAPSIZE][MAPSIZE]) {
			for (int i = 0; i < MAPSIZE; ++i)
				for (int j = 0; j < MAPSIZE; ++j)
					data[i][j] = map[i][j];
		}

		void loadFromFile(const std::string &filename) {
			std::ifstream f;
			f.open(filename);
			if (!f.is_open())
				throw std::runtime_error("Can't open" + filename);

			int lineCount = 0;
			while (f.peek() != EOF) {
				int length, row, col;
				std::string direction;
				f >> length >> row >> col >> direction;
				row -= 1;
				col -= 1;
				switch(this->recognizeAndSetShipSource(row, col, direction, length)) {
					case RecognizeError::NoError:
						break;
					case RecognizeError::IncorrectDirection:
						throw std::runtime_error("Ошибка распознавания файла!\nСтрока:" + std::to_string(lineCount) + " Неверное направление! Возможные: \"влево\", \"вправо\", \"вверх\", \"вниз\" \n");
					case RecognizeError::IncorrectCell:
						throw std::runtime_error("Ошибка распознавания файла!\nСтрока:" + std::to_string(lineCount) + " Неверная строка или/и столбец!\n");
					case RecognizeError::CellAlreadyUsed:
						throw std::runtime_error("Ошибка распознавания файла!\nСтрока:" + std::to_string(lineCount) + " Клетка с этим направлением уже занята!\n");
				}
				lineCount += 1;
				//TODO: прикрутить проверку что все корабли установлены
			}
		}

		void loadFromConsole() {
			auto ship = SHIPS.begin();
			while (true) {
				if (ship == SHIPS.end())
					break;

				cout << "Расставьте " << ship->count << " " << ship->name << ". Ваше поле:\n";
				this->print();

				cout << "Введите данные корабля в виде \"СТРОКА СТОЛБЕЦ НАПРАВЛЕНИЕ\". Например: \"1 1 вправо\"\n";
				int row, col;
				std::string direction;
				cin >> row >> col >> direction;

				row -= 1;
				col -= 1;

				switch(this->recognizeAndSetShipSource(row, col, direction, ship->length)) {
					case RecognizeError::NoError:
						ship->count -= 1;
						break;
					case RecognizeError::IncorrectDirection:
						cout << "Неверное направление! Возможные: \"влево\", \"вправо\", \"вверх\", \"вниз\" \n";
						break;
					case RecognizeError::IncorrectCell:
						cout << "Неверная строка или/и столбец!\n";
						break;
					case RecognizeError::CellAlreadyUsed:
						cout << "Клетка с этим направлением уже занята!\n";
						break;
				}

				if (ship->count == 0)
					++ship;
			}
		}

		RecognizeError recognizeAndSetShipSource(int row, int col, const std::string &stringDirection, int length) {
			ShipDirection direction;
			if (this->isValidCell(row, col)) {
				if (stringDirection == "вправо" || stringDirection == "право") {
					direction = ShipDirection::RIGHT;
				}else if (stringDirection == "влево" || stringDirection == "лево") {
					direction = ShipDirection::LEFT;
				}else if (stringDirection == "вверх" || stringDirection == "верх") {
					direction = ShipDirection::UP;
				}else if (stringDirection == "вниз" || stringDirection == "низ") {
					direction = ShipDirection::DOWN;
				} else {
					return RecognizeError::IncorrectDirection;
				}

				if (this->setShipIfCanPlace(row, col, direction, length)) {
					return RecognizeError::NoError;
				}else{
					return RecognizeError::CellAlreadyUsed;
				}
			}
			return RecognizeError::IncorrectCell;
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

		bool setShipIfCanPlace(int row, int col, ShipDirection direction, int length) {
			if (!this->isValidCell(row, col)) {
				cout << "INVALID\n";
				return false;
			}

			switch (direction) {
				case ShipDirection::LEFT:
					if (col - length < 0)
						return false;

					for (int i = 0; i < length; ++i)
						if (!this->canPlaceShipInCell(row, col-i))
							return false;

					for (int i = 0; i < length; ++i)
						this->data[row][col-i] = true;
					break;

				case ShipDirection::UP:
					if (row - length < 0)
						return false;

					for (int i = 0; i < length; ++i)
						if (!this->canPlaceShipInCell(row-i, col))
							return false;

					for (int i = 0; i < length; ++i)
						this->data[row-i][col] = true;
					break;

				case ShipDirection::RIGHT:
					if (col + length >= MAPSIZE)
						return false;

					for (int i = 0; i < length; ++i)
						if (!this->canPlaceShipInCell(row, col+i))
							return false;

					for (int i = 0; i < length; ++i)
						this->data[row][col+i] = true;
					break;

				case ShipDirection::DOWN:
					if (row + length >= MAPSIZE)
						return false;

					for (int i = 0; i < length; ++i)
						if (!this->canPlaceShipInCell(row+i, col))
							return false;

					for (int i = 0; i < length; ++i)
						this->data[row+i][col] = true;
					break;

				default:
					return false;
			}
			return true;
		}

		bool canPlaceShipInCell(int row, int col) {
			for (int i = -1; i < 2; ++i) {
				for (int j = -1; j < 2; ++ j) {
					if (!this->isValidCell(row+i, col+j))
						continue;
					if (this->data[row+i][col+j])
						return false;
				}
			}
			return true;
		}

		bool isValidCell(int row, int col) const {
			if (row < 0 || row >= MAPSIZE || col < 0 || col >= MAPSIZE)
				return false;
			return true;
		}

		void print() const {
			cout << (*this);
		}

		friend void operator<<(std::ostream &stream, const Map &m) {
			stream << "   "; // Красивый отступ

			// Штука в том, что символ юникод не влазит в один char, так что храним в массиве char'ов
			// При необходимости итерирования к следующей буковке - увеличиваем элемент с первым индексом

//			char alphabetLetter[8]; // Тут будем хранить текуую букву

//			// проблема в том, что "\u0430" - константный массив. А нам нужно менять первый элемент. Поэтому копируем в изменяемый массив:
//			for (int i = 0; i < 8; ++i)
//				alphabetLetter[i] = "\u0430"[i]; // Символ а в юникоде
//			alphabetLetter[1] += -1; // Из-за того, что в циклы мы сперва меняем букву приходится начинать с -1 позиции

//			// Выводим наши буквы на экран
//			for (int i = 0; i < MAPSIZE; ++i) {
//				// Пропускаем букву й
//				if (i == MAPSIZE-1)
//					alphabetLetter[1] += 1;

//				// Меняем символ на последующий
//				alphabetLetter[1] += 1;

//				cout << alphabetLetter << " ";
//			}
			for (int i = 97; i < MAPSIZE+97; ++i)
				std::cout << char(i) << " ";
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

		void clear() {
			for (int i = 0; i < MAPSIZE; ++i)
				for (int j = 0; j < MAPSIZE; ++j)
					this->data[i][j] = false;
		}

		bool get(int row, int col) const {
			if (!this->isValidCell(row, col))
				return false;
			return data[row][col];
		}


	private:
		bool data[MAPSIZE][MAPSIZE];
		// Строка - название корабля,
		// первое число - количество кораблей,
		// Второе число - длина корабля
		std::vector<ShipInfo> SHIPS = {
			{"Четырёхпалубник", 1, 4},
			{"Трёхпалубник", 2, 3},
			{"Двухпалубник", 3, 2},
			{"Однопалубник", 4, 1}
		};
};

class Player {
	public:
		Player() {
			if (std::filesystem::is_regular_file(std::filesystem::current_path().c_str()+std::string("/../input.txt"))) {
				this->shipMap.loadFromFile(std::filesystem::current_path().c_str()+std::string("/../input.txt"));
			}else{
				this->shipMap.loadFromConsole();
			}
		}

		void attack() {
			std::cout << "Введите поле для атаки (\"а1\"): ";
			std::string s;
			std::cin >> s;
			if (int(s[0]) < 97 || int(s[0]) >= 97 + MAPSIZE || std::stoi(s.substr(1)) < 1 || std::stoi(s.substr(1)) > MAPSIZE) {
				std::cout << s << " " << std::stoi(s.substr(1)) << " " << int(s[0]) << "\n";
				std::cout << "Введены неверные данные!\n\tДоступные:" << char(97) << "1-" << char(97+MAPSIZE) << MAPSIZE << ")";
				return;
			}
		}

		void printMaps(){
			std::cout << "Корабли игрока:\n";
			this->shipMap.print();
			std::cout << "Атакованные поля:\n";
			this->attackMap.print();
		}

	private:
		Map shipMap;
		Map attackMap;
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

	Map botMap;
	botMap.generateRandomMap();
	botMap.print();

	Player player;
	player.printMaps();
	player.attack();

	return 0;
}
