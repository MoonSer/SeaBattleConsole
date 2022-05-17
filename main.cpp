#include <iostream>			// Нужно для вывода в консоль
#include <filesystem>		// Для обнаружения файла с картой
#include <fstream>			// Для считывания карты из файла
#include <cmath>			// Для sqrt ( корень )
#include <random>			// Для логики генерации карты и атаки бота
#include <string>			// Для работы со строками
#include <vector>			// Массив данных vector - для списка истории атак
#include <map>				// Массив данных map - для списка истории атак

using namespace std;

#define MAPSIZE 10		// Размер карты
#define SHIPSCOUNT 10
		
// Объект для хранения информации о корабле
struct ShipInfo {
	string name;
	int count;
	int length;
};

// Вектор со всеми возможными кораблями
// Строка - название корабля,
// первое число - количество кораблей,
// Второе число - длина корабля
vector<ShipInfo> AVAILABLE_SHIPS {
	{"Четырёхпалубник", 1, 4},
	{"Трёхпалубник", 2, 3},
	{"Двухпалубник", 3, 2},
	{"Однопалубник", 4, 1}
};

// Статус клетки
enum CellType {
	EMPTY,  		// Пустая
	SHIP, 			// С кораблем
	ATTACKED, 		// Атакованая, "мимо"
	ATTACKEDSHIP, 	// Атакованая, попали в корабль
	KILLEDSHIP		// Убитый корабль
};

// Напраление корабля, нужно для расставления и случайной генерации (для бота)
enum ShipDirection {
	LEFT, UP, RIGHT, DOWN
};

// Объект который представляет собой одну клетку корабля
// Содержит позицию и тип ячейки
// структура - это класс, но имеющий публичный доступ к переменным
struct ShipPart {
	// Стандартный конструктор,  r - строка, c - столбец, t - тип ( ранен, убит, пуст)
	ShipPart(int r, int c, CellType t) : row(r), column(c), type(t) {}

	// Метод класса ShipPart, возвращает true, если данная часть находится в клетке ( клетка задаётся аргументами )
	bool inCell(int r, int c) const {
		return r == this->row && c == this->column;
	}

	// Расчитывает дистанцию до другой клетки, r - строка, c - столбец
	float distanceTo(int r, int c) {
		// Обычная формула расчёты длины между двумя точками через длину вектора
		auto val = sqrt((this->row-r)*(this->row-r) + (this->column-c)*(this->column-c));
		return val;
	}

	// Метод класса ShipPart, Устанавливает свойство (переменную) класса в переданную.
	// Используется, если нужно установить что часть корабля ранена/убита
	void updateState(CellType type) {
		this->type = type;
	}

	// Метод класса ShipPart, возвращет истину, если клетка была атакована, но корадль не убит
	bool isAttacked() const {
		return (this->type == CellType::ATTACKEDSHIP);
	}

	int row;
	int column;
	CellType type;
};


// Объект который представляет собой корабль
// Содержит вектор(массив) из клеток (частей) корабля
class Ship {
	public:
		// Стандартный конструктор, создаёт корабль с началом в клетке row, column длиной length и направлением direction
		Ship(int row, int column, int length = 1, ShipDirection direction = ShipDirection::RIGHT) {
			// Проходимся от 0 до длины и, в зависимости от направления, добавляем в вектор с клетками корабля новые
			for (int i = 0; i < length; ++i) {
				// Смотрим, какое направление
				switch (direction) {
					// Если направление вниз - то уменьшаем номер столбца
					case ShipDirection::LEFT:
						this->shipParts.emplace_back(row, column-i, CellType::SHIP); // Сама операция добавления
						break;
					// Если направление вниз - то уменьшаем номер строки
					case ShipDirection::UP:
						this->shipParts.emplace_back(row-i, column, CellType::SHIP);// Сама операция добавления
						break;
					// Если направление вниз - то увеличиваем номер столбца
					case ShipDirection::RIGHT:
						this->shipParts.emplace_back(row, column+i, CellType::SHIP);// Сама операция добавления
						break;
					// Если направление вниз - то увеличиваем номер строки
					case ShipDirection::DOWN: 
						this->shipParts.emplace_back(row+i, column, CellType::SHIP);// Сама операция добавления
						break;
				}
			}
		}

		// Конструктор, создаёт корабль в клетке row, column и с определённым типом
		// Например, если при атаке мы атакуем единичный корабль - то пользуемся этим конструктором и сразу создаём убитый
		Ship(int row, int col, CellType type) {
			this->shipParts.emplace_back(row, col, type);
		}

		// Метод класса Ship, возвращет истину, корабль располагается на клетке row, column
		// Иначе - ложь.
		bool contains(int row, int column) const {
			// Проходимся по каждой клетке (части)
			for (auto &shipPart : this->shipParts) {
				// И, если часть (клетка) находится в клетке row, column
				if (shipPart.inCell(row, column))
					return true; // Возвращаем истину
			}
			return false;
		}

		// Возвращает тип части (клетки) корабля, который находится в клетке row, column
		CellType partState(int row, int column) const {
			for(auto &shipPart : this->shipParts) {
				if (shipPart.column == column && shipPart.row == row)
					return shipPart.type;
			}
			// Если корабль не содержит клетку {row, column}, то выбрасываем исключение - в программе что-то не так.
			throw runtime_error("Incorrect ship cell");
		}

		// Принимаем атаку на корабль в клетке {row, column}
		CellType applyAttack(int row, int column) {
			// Проходимся ао каждой части корабля
			for (auto &shipPart : shipParts ) {
				// И, если нашли часть, которая находится в клетке {row, column}
				if (shipPart.inCell(row, column)) {
					// То обнавлем клетку корабля на атакованную
					shipPart.updateState(CellType::ATTACKEDSHIP);
					// Теперь, необходимо проверит, умирает ли корабль после попадания
					if (this->isAllShipPartsAttacked()) {
						// Если все части корабля были атакованы - "убиваем" корабль
						this->kill(); // Ну то есть устанавливаем тип у всех клеток корабля в KILLEDSHIP
						// Возвращаем тип, если корабль убит - то KILLEDSHIP
						return CellType::KILLEDSHIP;
					}
					// Если не все клетки корабля были атакованы - то возвращаем ATTACKEDSHIP
					return CellType::ATTACKEDSHIP;
				}
			}
		}

		// Метод класса, который проверяет, все ли части корабля были атакованы
		bool isAllShipPartsAttacked() const {
			for (auto &shipPart : this->shipParts) {
				// Если во время перебора встречаем не атакованную часть - то возвращаем ложь
				if (!shipPart.isAttacked())
					return false;
			}
			return true;
		}

		// Метод класса, который проверяет, был ли убит корабль
		bool isKilled() const {
			for (auto &shipPart : this->shipParts) {
				// Если во время перебора встречаем не убитую часть - то возвращаем ложь
				if (shipPart.type != CellType::KILLEDSHIP) 
					return false;
			}
			return true;
		}

		// Метод класса, который убивает текущий корабль
		void kill()  {
			// Проходимся по каждой части и устанавливаем тип в убитый
			for (auto &shipPart : shipParts )
				shipPart.updateState(CellType::KILLEDSHIP);
		}

		// Возвращает все части текущего корабля
		vector<ShipPart> parts() const {
			return this->shipParts;
		}

		// Рассчитывает минимальную дистанцию до другого корабля
		float minimalDistanceTo(int row, int col) {
			// Просто проходимся по каждой части корабля и ищем минимальную дистанцию.
			float minDistance = 10;
			for (auto &shipPart : this->shipParts) {
				// Рассчитываем дистанцию от текущей точки {row, col} до перебираемой части корабля
				float distance = shipPart.distanceTo(row, col);
				if ( distance < minDistance)
					minDistance = distance;
			}
			return minDistance;
		}
	
		// Метод для добавления кораблю новую часть. Необходимо для карты с атакованными кораблями.
		void addPart(int row, int column, CellType type) {
			this->shipParts.emplace_back(row, column, type);
		}


	private:
		// Собственно сам вектор(массив) с частями корабля
		vector<ShipPart> shipParts;
};


// Объект который представляет собой карту
// Содержит вектор(массив) из кораблей
class Map {
	public:
		// Enum - перечисление, элемент перечисления - просто число, например NoError - число 0, IncorrectDirection - 1.
		// Необходимо для распознавания ошибки
		enum RecognizeError {
			NoError, IncorrectDirection, IncorrectCell, CellAlreadyUsed
		};


		// Метод обрабатывает атаку на нас в клетку {row, column}
		CellType cellAttacked(int row, int column) {
			// Проходимся по всем кораблям и ищем тот, часть которого находится в заданной клетке
			for (auto &ship : this->ships) {
				if (ship.contains(row, column)) {
					// Если находим, то принимаем атаку на корабль и возвращаем ответ (ранен, убит)
					return ship.applyAttack(row, column);
				}
			}
			// Если не находим, то проверяем, возможно эта клетка уже была атакована
			// Эта проверка нужна для того, чтобы в нашем словаре (вектор из пар ключ:значения) не было дубликатов
			// Проще говоря, что бы например клетка {1, 0} по сто раз не вносилась в словарь
			if (!this->isAlreadyInVoidAttacks(row, column))
				// Если же мы ещё не вносили в наш словарь данную клетку, то вносим:
				this->voidAttacks.insert({row, column});
			
			// Собственно значит что заданная клетка была пуста -> теперь она атакована (мимо)
			return CellType::ATTACKED;
		}

		// Метод обрабатывает нашу атаку в клетку {row, column} с результатом type
		void applyAttack(int row, int column, CellType type) {
			// Если результат - попали, но мимо, то
			if (type == CellType::ATTACKED) {
				// Осталось проверить, не добавляли ли мы уже ету клетку в перечень атакованных
				if (!this->isAlreadyInVoidAttacks(row, column))
					// Если не добавляли - то добавляем
					this->voidAttacks.emplace(row, column);
			}else {
				// Такс, раз мы тут - то корабль либо убит либо ранен
				// Перебираем все корабли
				for (auto &ship : this->ships) {
					// Перебираем все части корабля
					for (auto &part : ship.parts()) {
						// Ищем, не находится ли ячейка {row, column} впритык с уже атакованным кораблем. 
						// Проще говоря, на карте атак изначально мы не знаем расположение кораблей на карте у другого игрока
						// Поэтому, если корабль был ранен/убит до этого, то ищем, является ли клетка {row, column} частью уже атакованного корабля
						if ((part.row == row && abs(part.column - column) == 1) || (part.column == column && abs(part.row - row) == 1) ) {
							// Если является, то добавляем его к уже существующему
							ship.addPart(row, column, CellType::ATTACKEDSHIP);
							// Далее, если же результатом атаки - KILLEDSHIP ( то есть это была последняя живая часть корабля)
							if (type == CellType::KILLEDSHIP)
								// Мы помечаем весь корабль как мёртвый
								ship.kill();
							// return нужен, чтобы исполнение программы не пошло дальше - кораблике мы уже добавили, создавать новый не нужно
							return;
						}
					}
				}
				// Выше мы искали, является ли часть корабля из клетки {row, column} частью другого, атакованного до этого корабля
				// Если такого корабля нет (мы бьём по нему первый раз)
				// То просто добавляем новый единичный корабль в наш списочек.
				this->ships.emplace_back(row, column, type);
			}
		}

		// Метод для проверки, били ли мы уже в эту точку.
		bool isAlreadyInVoidAttacks(int row, int column) const {
			// Ищем номера столбцов всех  атакованных ячеек, строка которых равна row
			auto allValuesForGivenRow = this->voidAttacks.equal_range(row);
			// Теперь, итерируясь по всем таким элементам
			for (auto it = allValuesForGivenRow.first; it != allValuesForGivenRow.second; ++it) {
				// Проверяем, содержится ли в перечне атакованных клеток наша клетка {row, column}
				if (it->first == row && it->second == column) {
					// Если содержится - возвращаем истину
					return true;		
				}
			}
			// Иначе - ложь
			return false;
		}

		// Метод, который загружает карту из файла, имя файла - передаётся в переменной filename
		void loadFromFile(const string &filename) {
			// ifstream - объект, который позволяет работать с файлами.
			ifstream f;
			// Открываем наш файлик
			f.open(filename);
			// Если не получилось - выбрасываем исключение ( программа вырубается )
			if (!f.is_open())
				throw runtime_error("Can't open" + filename);

			// Счётчик строк нужен, чтобы если вдруг ошибка при чтении файла - знать на какой строке эта ошибка. 
			int lineCount = 0;
			// peek() - возвращает, но не считывает из буфера следующий символ
			while (f.peek() != EOF) {
				// Создаём переменные чтобы в них можно было считать ввод из файла
				int length, row, column;
				string direction;
				// Считываем Данные в переменные
				f >> length >> row >> column >> direction;
				// Вычитаем единицу, потому что нумерация в массивах начинается с 0, а в морском бое - с 1.
				row -= 1;
				column -= 1;
				// Пытаемся распознать направление и, если получится - создать корабль.
				switch(this->recognizeAndSetShipSource(row, column, direction, length)) {
					// Вод для это штуки и нужны enum ( перечисления ) - так удобней распознавать, какие были ошибки
					case RecognizeError::NoError: // Всё норм, ошибок нет
						break; // Выходим
					case RecognizeError::IncorrectDirection: // Ай-ай-ай! Не получилось распознть направление
						throw runtime_error("Ошибка распознавания файла!\nСтрока:" + to_string(lineCount) + " Неверное направление! Возможные: \"влево\", \"вправо\", \"вверх\", \"вниз\" \n");
						break;
					case RecognizeError::IncorrectCell:// Ошибка - неправильная строка/столбец
						throw runtime_error("Ошибка распознавания файла!\nСтрока:" + to_string(lineCount) + " Неверная строка или/и столбец!\n");
						break;
					case RecognizeError::CellAlreadyUsed: // Ошибка - в этой клетке нельзя разместить корабль, т.к. рядом находится другой!
						throw runtime_error("Ошибка распознавания файла!\nСтрока:" + to_string(lineCount) + " Клетка с этим направлением уже занята!\n");
						break;
				}
				// Если программа не вызвала ни один throw ( а тогда она прекратит выполнение и крашнется с ошибкой ), то увеличиваем счётчик строк, всё норм распозналось
				lineCount += 1;
			}
			// Если из файла считалось недостаточно кораблей - вызываем ошибку.
			if (this->ships.size() != SHIPSCOUNT)
				throw runtime_error("Мало кораблей!");
		}

		// Метод, который загружает карту из консоли (запрашивает данные у пользователя)
		void loadFromConsole() {
			// Чтобы не напортачить с количеством кораблей, у нас есть их полный изначальный список (AVAILABLE_SHIPS)
			//  Поэтому будем по нему итерироваться (.begin() - возвращает итератор на первый элемиент, .end() - итератор на элемент после последнего).
			// Через ++<итератор> - можно переходить к следующему элементу
			auto ship = AVAILABLE_SHIPS.begin();
			// Цикл бесконечный, потому что, если посльзователь введёт что-то неправильно, 
			// Можно будет просто ещё раз запустить этот же цикл, не увеличивая итератор.
			while (true) {
				// Если итератор достиг конца - то обрываем цикл
				if (ship == AVAILABLE_SHIPS.end())
					break;

				cout << "Расставьте " << ship->count << " " << ship->name << ". Ваше поле:\n";
				// print - печатает текущую карту кораблей (чтобы можно было ориентироваться, куда ставить)
				this->print();

				// Запрашиваем у пользователя данные о корабле
				cout << "Введите данные корабля в виде \"СТРОКА СТОЛБЕЦ НАПРАВЛЕНИЕ\". Например: \"1 1 вправо\"\n";

				// Как и со считыванием из файла - создаём переменные для считвания в них
				int row, col;
				string direction;
				// Считываем Данные в переменные
				cin >> row >> col >> direction;
				// Вычитаем единицу, потому что нумерация в массивах начинается с 0, а в морском бое - с 1.
				row -= 1;
				col -= 1;
				// Пытаемся распознать направление и, если получится - создать корабль.
				switch(this->recognizeAndSetShipSource(row, col, direction, ship->length)) {
					case RecognizeError::NoError: // Всё норм, ошибок нет
						// Раз ошибок нет - уменьшаем количество кораблей
						ship->count -= 1;
						break;
					case RecognizeError::IncorrectDirection: // Ай-ай-ай! Не получилось распознть направление
						cout << "Неверное направление! Возможные: \"влево\", \"вправо\", \"вверх\", \"вниз\" \n";
						break;
					case RecognizeError::IncorrectCell:// Ошибка - неправильная строка/столбец
						cout << "Неверная строка или/и столбец!\n";
						break;
					case RecognizeError::CellAlreadyUsed:// Ошибка - в этой клетке нельзя разместить корабль, т.к. рядом находится другой!
						cout << "Клетка с этим направлением уже занята!\n";
						break;
				}
				// Если программа не вызвала ни один throw ( а тогда она прекратит выполнение и крашнется с ошибкой ), 
				// то проверяем, про все ли корабли данного типа мы расставили
				if (ship->count == 0)
					++ship;  // ship - итератор, поэтому с помощью ++ переходим к следующему элементу
			}
		}

		// Метод класса Map, распознаёт введённые данные корабля и, если всё верно - создаёт корабль
		RecognizeError recognizeAndSetShipSource(int row, int column, const string &stringDirection, int length) {
			// В эту переменную положим распознанное значение направления
			// Проще говоря, считать мы можем только строку, а для удобства - нам нужно зеначения перечисления ShipDirection.
			ShipDirection direction;
			// Первая проверка - валидная ли это ячейка
			if (this->isValidCell(row, column)) {
				// Собственно сам процесс распознания, через if узнаём, какое направление и лоэим в переменную direction
				if (stringDirection == "вправо" || stringDirection == "право") {
					direction = ShipDirection::RIGHT;
				}else if (stringDirection == "влево" || stringDirection == "лево") {
					direction = ShipDirection::LEFT;
				}else if (stringDirection == "вверх" || stringDirection == "верх") {
					direction = ShipDirection::UP;
				}else if (stringDirection == "вниз" || stringDirection == "низ") {
					direction = ShipDirection::DOWN;
				} else {
					// Если ни один if не сработает, возвращаем результат, что направление неверное 
					return RecognizeError::IncorrectDirection;
				}
				// Если мы дошли до этого момента - успешно распознали direction!
				// Вызываем другой метод, который устанавливает корабль в ячейку {row, column} с длиной length и направлением direction,
				// Если ячейка свободна. Метод setShipIfCanPlace возвращает true, если корабль успешно установлен
				if (this->setShipIfCanPlace(row, column, direction, length)) {
					return RecognizeError::NoError;	// Раз мы тут - функция вернула true - возвращаем RecognizeError::NoError
				}else{
					// Если мы здесь - фукнция вернула false, значит ячейка уже занята((
					return RecognizeError::CellAlreadyUsed;
				}
			}
			// Еслия ячейка невалидна - возвращаем соответственное значение перечисления
			return RecognizeError::IncorrectCell;
		}

		// Генерирует рандомные корабли для бота
		void generateRandomMap() {
			// Генератор рандомных чисел
			random_device dev;
			mt19937 rng(dev());
			// Объект для генерации рандомных чисел
			uniform_int_distribution<mt19937::result_type> randomMapNumber(0, MAPSIZE-1);
			uniform_int_distribution<mt19937::result_type> randomDirection(0, 3);
			// Поэлементно проходимся по всем возможным (необходимым для генерации) кораблям
			for (auto &ship:AVAILABLE_SHIPS)
				// Итерируемся по количеству кораблей
				for (int shipsCount = 0; shipsCount < ship.count; ++shipsCount)
					// И пытаемся поставить корабль в случайно сгенерированную клетку со случайным направлением
					// Здесь используется while, потому что setShipIfCanPlace возвращает false, если не получилось 
					// установаить корабль. Проще говоря, строка ниже повторяется до тех пор, пока случайно сгенерированный корабль не поместится на карте
					while (!this->setShipIfCanPlace(randomMapNumber(rng), randomMapNumber(rng), ShipDirection(randomDirection(rng)), ship.length));
		}

		// Устанавливает корабль в row, col с направленеи direction и длиной length, если эьт клетки не заняты
		bool setShipIfCanPlace(int row, int column, ShipDirection direction, int length) {
			// Первым делом,  проверяем чтобы начало корабля (начальная клетка) была валидной
			if (!this->isValidCell(row, column))
				return false;

			// Теперь, в зависимости от направления, проверяем что корабль поместится на карте
			switch (direction) {
				case ShipDirection::LEFT:
					if (column - length < 0) // Если корабль направлен влево, то его длина должна быть больше левой границы карты
						return false;
					
					// Продимся по длине и проверяем, все ли части корабля мы можем установить
					for (int i = 0; i < length; ++i) // Т.К. направление влево, то уменьшаем столбец
						if (!this->canPlaceShipInCell(row, column-i))
							return false;
					break;

				case ShipDirection::UP:
					if (row - length < 0) // Если корабль направлен вверх, то его длина должна быть больше верхней границы карты
						return false;

					// Продимся по длине и проверяем, все ли части корабля мы можем установить
					for (int i = 0; i < length; ++i) // Т.К. направление вверх, то уменьшаем строку
						if (!this->canPlaceShipInCell(row-i, column))
							return false;
					break;

				case ShipDirection::RIGHT:
					if (column + length >= MAPSIZE) // Если корабль направлен вправо, то его длина должна быть меньше правой границы карты
						return false;

					// Продимся по длине и проверяем, все ли части корабля мы можем установить
					for (int i = 0; i < length; ++i) // Т.К. направление вправо, то увеличиваем столбец
						if (!this->canPlaceShipInCell(row, column+i))
							return false;
					break;

				case ShipDirection::DOWN:
					if (row + length >= MAPSIZE) // Если корабль направлен вниз, то его длина должна быть меньше нижней границы карты
						return false;

					// Продимся по длине и проверяем, все ли части корабля мы можем установить
					for (int i = 0; i < length; ++i)// Т.К. направление вниз, то увеличиваем строку
						if (!this->canPlaceShipInCell(row+i, column))
							return false;
					break;
			}
			// Раз программа выполнилась до этого момента, значит всё ок и корабль можно установить.
			// Собственно это мы и сделаем - добавим в вектор (массив) с кораблями новый корабль:
			this->ships.push_back({row, column, length, direction});
			return true;
		}

		// Возвращает true, если ни один уже установленный корабль не мешает установить новый одиночный корабль в клетку {row, column}
		bool canPlaceShipInCell(int row, int column) {
			// Проходимся по каждму кораблю
			for (auto &ship : this->ships) {
				// И смотрим длину от заданной клетки {row, column} до текущего корабля ship
				// Она должна быть больше или равна 2, потому что именно такое расстояние минимально между двумя кораблями
				if (ship.minimalDistanceTo(row, column) < 2)
					return false;
			}
			// Раз программа долшла до сюда - всё ок, возвращаем истину, корабль можно разместить
			return true;
		}

		// Проверяет ячейку {row, column} на валидность
		bool isValidCell(int row, int column) const {
			// Ячейка валидна, если {row, column} больше или равна нулю и меньше размера карты
			if (row < 0 || row >= MAPSIZE || column < 0 || column >= MAPSIZE)
				return false;
			return true;
		}

		// Выводит на экран нашу карту
		void print() const {
			// Выводим заголовок карты (a b c d, ...)
			this->printHeader();
			cout << "\n";
			// Выводим основную карту
			for (int i = 0; i < MAPSIZE; ++i) {
				// Выводим на консоль i-ю строку
				this->printRow(i);
				cout << "\n";
			}
		}

		// Выводит на экран заголовок карты
		void printHeader() const {
			cout << "   "; // Красивый отступ
			// 97 - номер символа a в кодировке ASCII
			for (int i = 97; i < MAPSIZE+97; ++i)
				cout << char(i) << " "; // Превращая число в символ, выводим заголовок карты 
		}

		// Выводит на экран строку карты
		void printRow(int row) const {
			// Т.к. у нас итарция начинается с 0, а в карте - должна быть с единицы, выводим row+1
			cout << (row+1);

			// Если мы печатаем не 10-ю строку (9-ю если считать для нашей итерации с 0), 
			// то надо вывести доп. отступ
			// 9_ <- "_" - это отступ
			// 10
			if (row != 9) {
				cout << " ";
			}
			cout  <<"|";

			// Вот тут мы проходимся по каждому столбцу, и выводим символ в зависимости от типа клетки
			for (int column = 0; column < MAPSIZE; ++column){
				// get(row, column) - возвращает тип клетки {row, column} (атакована, пустая, мимо, ...)
				switch (this->get(row, column)) {
					case CellType::EMPTY: // Клетка пустая
						cout << " ";
						break;
					case CellType::SHIP: // В клетке находится корабль
						cout << "#";
						break;
					case CellType::ATTACKEDSHIP: //  В клетке находится атакованный корабль 
						cout << "X";
						break;
					case CellType::ATTACKED:  // Клетка атакована, но корабля неиту (мимо)
						cout << "*";
						break;
					case CellType::KILLEDSHIP: //  В клетке находится убитый корабль 
						cout << "┼";
						break;
				}
				cout << "|";
			}
		}

		// Возвращает тип клетки {row, column} (атакована, пустая, с кораблём, ...)
		CellType get(int row, int column) const {
			// Первым делом проверяем, что клетка валидна
			if (!this->isValidCell(row, column))
				// Если не валидна, то возвращаем пустую ()
				return CellType::EMPTY;
			
			// Теперь, если клетка валидна, то проходимся по всем кораблям и проверяем
			for (auto ship : this->ships) {
				// находится ли какая-нибудь часть корабля в этой клетке
				if (ship.contains(row, column)) {
					// Если нашли корабль - то возвращаем состояние части корабля, которая находится в запрашиваемой клетке {row, column}
					return ship.partState(row, column);
				}
			}
			
			// Раз исполнение долшо до сюда - клетка валидна, но корабля в ней нету
			// Значит проходимся по атакованным клеткам и проверяем, может её атаковали
			for (auto cell : this->voidAttacks) {
				// Если действительно клетка была атакована - возвращаем соответствующее состояние клетки(мимо)
				if (cell.first == row && cell.second == column)
					return CellType::ATTACKED;
			}
			// Раз выполниние дошло до сюда, то эта пустая клетка, в ней нет корабля и она не была атакована
			return CellType::EMPTY;
		}

		// Проверяет, все ли корабли были побеждены
		bool isAllShipsDefeated() const {
			// Просто проходимся по всем кораблям
			for (auto &ship : this->ships) {
				// И проверяем, убиты ли они. Если находим хотя бы один не убитый
				if (!ship.isKilled())
					// Возвращаем ложь
					return false;
			}
			return true;
		}

	private:
		// Вектор (массив) с кораблями
		vector<Ship> ships;
		// Вектор пар ключ:значение, хранит все клетки по которым мы стреляли и не попали
		// multimap отличается от map тем, что для одного ключа может хранить несколько значений
		multimap<int, int> voidAttacks;
};



// Объект который представляет собой базовый класс для игрока и бота
// Содержит базовую логику, которая одинаковая и у бота и у игрока
class ControlEntity {
	public:
		// Конструктор, который сохраняет имя игрока в поле entityName
		// Это имя используется при выводе карты
		ControlEntity(const string& name) : entityName(name) {}
		
		// Выводит на экран наши карты (карта кораблей и карту наших атак)
		void printMaps(){
			// Вывродим имя
			cout << "[*] " << this->entityName << "\n";
			
			// Выводим заголовок для карты кораблей
			this->shipMap.printHeader();
			cout << "       ";  // Красивый отступ между картами

			// Выводим заголовок для карты атак
			this->attackMap.printHeader();
			cout << "\n";

			// Выводим карты
			// проходя по каждой строке в наших картах,
			for (int row = 0; row < MAPSIZE; ++row) {
				// Сперва выводим соответствующую строку с картой кораблей
				this->shipMap.printRow(row);
				cout << "       ";// Красивый отступ между картами
				
				// Затем соответствующую строку карты с атаками
				this->attackMap.printRow(row);
				cout << "\n"; // Переходим на новую строчку
			}
		}

		// Метод обрабатывает атаку на нас в клетку {row, column}
		CellType applyAttackAtShipMap(int row, int column) {
			// Вызвыается соответствующий метод, который убивает/ранит/помечает клетку на карте кораблей и возвращает результат, какая клетка теперь
			// допустим, если вызывать этот метод для пустой клетки, то она станет помеченой как ATTACKED, то есть атакована мимо
			// если для для не последней дивой клетки корабля, то атакованная часть корабля станет помеченой как ATTACKEDSHIP, то есть ранена
			// если для последней живой клетки корабля, то атакованная корабль станет помеченый как KILLEDSHIP, то есть убит
			return this->shipMap.cellAttacked(row, column);
		}

		// Метод обрабатывает нашу атаку в клетку {row, column}. type - результат атаки, попали, мимо, убили, ...
		void applyAttackAtAttackMap(int row, int column, CellType type) {
			// Вызвыается соответствующий метод, который убивает/ранит/помечает клетку на карте атак
			this->attackMap.applyAttack(row, column, type);
		}

		// Проверяет, все ли корабли были побеждены
		bool isAllShipsDefeated() const {
			// Просто вызывает соответствующий метод у нашей карты
			return this->shipMap.isAllShipsDefeated();
		}

	protected:
		// Карта с кораблями
		Map shipMap;
		// Карта с атаками
		Map attackMap;

	private:
		// Имя игрока
		string entityName;
};



// Чтобы необходимая часть функциональности перекочевала в класс игрока - наследуемся
class Player : public ControlEntity {
	public:
		// Устанавливаем имя игрока как "Игрок"
		Player() : ControlEntity("Игрок") {
			// И проверяем, есть ли файл map.txt
			if (filesystem::is_regular_file(filesystem::current_path().append("map.txt"))) {
				// Если есть - считываем карту кораблей из неё
				this->shipMap.loadFromFile(filesystem::current_path().append("map.txt").generic_string());
			}else{
				// В противном случае запрашиваем у пользователя из консоли
				this->shipMap.loadFromConsole();
			}
		}

		// Метод, который запрашивает у пользователя ячейку для атаки
		pair<int, int> getAttackCell() const {
			cout << "Введите поле для атаки (\"а1\"): " ;
			// Считываем всю строку в переменную s
			string s;
			getline(cin, s);
			// И пытаемся распознать, в какую ячейку игрок хочет бить
			auto [row, column] = this->recognizeCellString(s);
			// Если ячейка не валидна
			if (!this->attackMap.isValidCell(row, column)) {
				// то выводим ошибку и запрашиваем клетку для атаки ещё раз.
				// Как писал выше, 96 - номер буквы "a" в ASCII-кодировке. Соответственно
				// 96+MAPSIZE - номер последнего столбца (для MAPSIZE=10 - 106 - номер буквы "j")
				cout << "Введены неверные данные!\n\tДоступные: " << "a1-" << char(96+MAPSIZE) << MAPSIZE << "\n";
				return this->getAttackCell();
			}
			// В противном случае просто возвращаем её
			return {row, column};
		}

		// Метод для распознавания строки и столбца, введённых пользователем
		pair<int, int> recognizeCellString(std::string_view s) const {
			try {
				// Пробуем преобразовать букву обратно в число
				int column = int(s[0])-97;
				// и просто преобразовать строкове представление число в тип int
				int row = stoi(s.substr(1).data())-1;
				// Если всё ок - то возвращаем распознанные строку и столбец
				return {row, column};
			// Блок catch выполнится только если вызовается какое-либо исключение (например пользователь ввёл строку и столбец в неправильном формате "1a")
			}catch(...) {
				// Возвращаем некорректные номера столбца и строки
				return {-1, -1};
			}
		}
};




// Чтобы необходимая часть функциональности перекочевала в классы бота - наследуемся
class Bot : public ControlEntity {
	
	// Структурка которая содержит строку и столбец, исользуется для истории атак
	struct AttackInfo {
		pair<int, int> cell() const {
			return {row, column};
		}
		int row;
		int column;
	};

	public:
		// Устанавливаем имя игрока как "Бот"
		Bot() : ControlEntity("Бот") {
			// Генерирует рандомную карту с кораблями
			this->shipMap.generateRandomMap();
		}

		// Метод, возвращает ячейку, в которую бот хочет атаковать 
		pair<int, int> getAttackCell() {
			// Генерируем случайную ячейку 
			auto [row, column] = this->generateRandomCell();
			// Добавляем её в историю атак
			this->attackHistory.push_back({row, column});
			return this->attackHistory.back().cell(); 
		}

		// Метод, генерирует случайную ячейку
		pair<int, int> generateRandomCell() const {
			// Инициализируем генератор случайных чисел
			random_device dev;
			mt19937 rng(dev());
			uniform_int_distribution<mt19937::result_type> randomMapNumber(0, MAPSIZE-1);
			// Генерируем случайную ячейку
			int row = randomMapNumber(rng);
			int column = randomMapNumber(rng);
			// Если её атакуем впервый раз то возвращаем
			if (!this->isAlreadyAttacked(row, column))
				return {row, column};
			// Иначе - перегенерируем ячейку
			return this->generateRandomCell();
		}
		
		// Метод, проверяет атаковал ли бот ячеку {row, column}
		bool isAlreadyAttacked(int row, int column) const {
			// Перебираем историю и смотрим, атаковали ли бот её до этого
			 for (auto &attack : this->attackHistory) {
				 if (attack.row == row && attack.column == column)
				 	return true;
			 }
			 return false;
		}

	private:
		vector<AttackInfo> attackHistory;	
};



int main() {
	cout << "                     _______. _______      ___       " << "\n"
		 << "                    /       ||   ____|    /   \\     " << "\n"
		 << "                   |   (----`|  |__      /  ^  \\    " << "\n"
		 << "                    \\   \\    |   __|    /  /_\\  \\" << "\n"
		 << "                .----)   |   |  |____  /  _____  \\  " << "\n"
		 << "                |_______/    |_______|/__/     \\__\\" << "\n";

	cout << "  .______        ___      .___________..___________. __       _______   " << "\n"
		 << "  |   _  \\      /   \\     |           ||           ||  |     |   ____|" << "\n"
		 << "  |  |_)  |    /  ^  \\    `---|  |----``---|  |----`|  |     |  |__    " << "\n"
		 << "  |   _  <    /  /_\\  \\       |  |         |  |     |  |     |   __|  " << "\n"
		 << "  |  |_)  |  /  _____  \\      |  |         |  |     |  `----.|  |____  " << "\n"
		 << "  |______/  /__/     \\__\\     |__|         |__|     |_______||_______|" << "\n";

	// Создаём объекты бота и игрока
	Bot bot;
	Player player;
	//  В бесконечном цикле
	while (true) {
		///// Если раскомментировать строку ниже - то карта бота тоже будет печататься
		// bot.printMaps();

		// Выводим карту игрока
		player.printMaps();

		{ // Запрашиваем ячеку у игрока
			auto [row, column] = player.getAttackCell();
			// Проводим атаку на бота и отображаем результат на карте атак игрока
			player.applyAttackAtAttackMap(row, column, bot.applyAttackAtShipMap(row, column));
		}
		{
			 // Запрашиваем ячеку у бота (генерируется случайно)
			auto [row, column] = bot.getAttackCell();
			// Выводим на экран, куда атаковал бот
			std::cout << "\nБот атакует в " << char(column+97) << row+1 << "\n\n";
			// Проводим атаку на игрока и отображаем результат на карте атак бота
			bot.applyAttackAtAttackMap(row, column, player.applyAttackAtShipMap(row, column));
		}

	
		// Проверяем, может у бота уже нет кораблей
		if (bot.isAllShipsDefeated()) {
			// Если у бота кончились корабли - выводим надпись, что вы победили!
			cout <<  " __   __  _______  __   __          _     _  ___   __    _  __   __   __ " << "\n"
				 <<  "|  | |  ||       ||  | |  |        | | _ | ||   | |  |  | ||  | |  | |  | " << "\n"
				 <<  "|  |_|  ||   _   ||  | |  |        | || || ||   | |   |_| ||  | |  | |  | " << "\n"
				 <<  "|       ||  | |  ||  |_|  |        |       ||   | |       ||  | |  | |  | " << "\n"
				 <<  "|_     _||  |_|  ||       |        |       ||   | |  _    ||__| |__| |__| " << "\n"
  				 <<  "  |   |  |       ||       |        |   _   ||   | | | |   | __   __   __  " << "\n"
				 <<  "  |___|  |_______||_______|        |__| |__||___| |_|  |__||__| |__| |__|" << "\n";
			break;// И завершаем цикл
		}
		// Проверяем, может у игрока уже нет кораблей
		if (player.isAllShipsDefeated()) {
			// Если у игрока кончились корабли - выводим надпись, что вы проиграли!
			cout << " __   __  _______  __   __          ___      _______  _______  _______ "<< "\n"
				 << "|  | |  ||       ||  | |  |        |   |    |       ||       ||       |"<< "\n"
				 << "|  |_|  ||   _   ||  | |  |        |   |    |   _   ||  _____||    ___|"<< "\n"
				 << "|       ||  | |  ||  |_|  |        |   |    |  | |  || |_____ |   |___ "<< "\n"
				 << "|_     _||  |_|  ||       |        |   |___ |  |_|  ||_____  ||    ___|"<< "\n"
  				 << "  |   |  |       ||       |        |       ||       | _____| ||   |___ "<< "\n"
  				 << "  |___|  |_______||_______|        |_______||_______||_______||_______|"<< "\n";
			break; // И завершаем цикл
		}
	}

	return 0;
}
