#include <iostream>
#include <random>
#include <vector>
#include <filesystem>
#include <fstream>
#include <string>

using namespace std;

#define MAPSIZE 10


class Map {


	public:
		
		struct ShipInfo {
			std::string name;
			int count;
			int length;
		};
		
		enum ShipDirection {
			LEFT, UP, RIGHT, DOWN
		};

		enum RecognizeError {
			NoError, IncorrectDirection, IncorrectCell, CellAlreadyUsed
		};

		enum CellType {
			EMPTY, SHIP, ATTACKED, ATTACKEDSHIP
		};

		Map() : data() {
			this->clear();
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
						throw std::runtime_error("�訡�� �ᯮ�������� 䠩��!\n��ப�:" + std::to_string(lineCount) + " ����୮� ���ࠢ�����! ��������: \"�����\", \"��ࠢ�\", \"�����\", \"����\" \n");
					case RecognizeError::IncorrectCell:
						throw std::runtime_error("�訡�� �ᯮ�������� 䠩��!\n��ப�:" + std::to_string(lineCount) + " ����ୠ� ��ப� ���/� �⮫���!\n");
					case RecognizeError::CellAlreadyUsed:
						throw std::runtime_error("�訡�� �ᯮ�������� 䠩��!\n��ப�:" + std::to_string(lineCount) + " ���⪠ � �⨬ ���ࠢ������ 㦥 �����!\n");
				}
				lineCount += 1;
				//TODO: �ਪ����� �஢��� �� �� ��ࠡ�� ��⠭������
			}
		}

		void loadFromConsole() {
			auto ship = SHIPS.begin();
			while (true) {
				if (ship == SHIPS.end())
					break;

				cout << "����⠢�� " << ship->count << " " << ship->name << ". ��� ����:\n";
				this->print();

				cout << "������ ����� ��ࠡ�� � ���� \"������ ������� �����������\". ���ਬ��: \"1 1 ��ࠢ�\"\n";
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
						cout << "����୮� ���ࠢ�����! ��������: \"�����\", \"��ࠢ�\", \"�����\", \"����\" \n";
						break;
					case RecognizeError::IncorrectCell:
						cout << "����ୠ� ��ப� ���/� �⮫���!\n";
						break;
					case RecognizeError::CellAlreadyUsed:
						cout << "���⪠ � �⨬ ���ࠢ������ 㦥 �����!\n";
						break;
				}

				if (ship->count == 0)
					++ship;
			}
		}

		RecognizeError recognizeAndSetShipSource(int row, int col, const std::string &stringDirection, int length) {
			ShipDirection direction;
			if (this->isValidCell(row, col)) {
				if (stringDirection == "��ࠢ�" || stringDirection == "�ࠢ�") {
					direction = ShipDirection::RIGHT;
				}else if (stringDirection == "�����" || stringDirection == "����") {
					direction = ShipDirection::LEFT;
				}else if (stringDirection == "�����" || stringDirection == "����") {
					direction = ShipDirection::UP;
				}else if (stringDirection == "����" || stringDirection == "���") {
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
						this->data[row][col-i] = CellType::SHIP;
					break;

				case ShipDirection::UP:
					if (row - length < 0)
						return false;

					for (int i = 0; i < length; ++i)
						if (!this->canPlaceShipInCell(row-i, col))
							return false;

					for (int i = 0; i < length; ++i)
						this->data[row-i][col] = CellType::SHIP;
					break;

				case ShipDirection::RIGHT:
					if (col + length >= MAPSIZE)
						return false;

					for (int i = 0; i < length; ++i)
						if (!this->canPlaceShipInCell(row, col+i))
							return false;

					for (int i = 0; i < length; ++i)
						this->data[row][col+i] = CellType::SHIP;
					break;

				case ShipDirection::DOWN:
					if (row + length >= MAPSIZE)
						return false;

					for (int i = 0; i < length; ++i)
						if (!this->canPlaceShipInCell(row+i, col))
							return false;

					for (int i = 0; i < length; ++i)
						this->data[row+i][col] = CellType::SHIP;
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
					if (this->data[row+i][col+j] != CellType::EMPTY)
						return false;
				}
			}
			return true;
		}

		void applyAttack(int row, int column, CellType type) {
			if (!this->isValidCell(row, column))
				throw std::runtime_error("Incorrect Cell:" + std::to_string(row) + " " + std::to_string(column));
			this->data[row][column] = type;	
		}

		bool isValidCell(int row, int col) const {
			if (row < 0 || row >= MAPSIZE || col < 0 || col >= MAPSIZE)
				return false;
			return true;
		}

		void print() const {
			std::cout << "   "; // ��ᨢ� �����
			for (int i = 97; i < MAPSIZE+97; ++i)
				std::cout << char(i) << " ";
			std::cout << "\n";
			// �뢮��� �����
			for (int i = 0; i < MAPSIZE; ++i) {
				std::cout << i+1 << (i == 9 ? "" : " ") <<"|";
				for (int j = 0; j < MAPSIZE; ++j){
					switch (this->get(i, j)) {
						case CellType::EMPTY:
							std::cout << " ";
							break;
						case CellType::SHIP:
							std::cout << "#";
							break;
						case CellType::ATTACKEDSHIP:
							std::cout << "X";
							break;
						case CellType::ATTACKED:
							std::cout << "*";
							break;
					}
					std::cout << "|";
				}
				std::cout << "\n";
			}
		}

		void clear() {
			for (int i = 0; i < MAPSIZE; ++i)
				for (int j = 0; j < MAPSIZE; ++j)
					this->data[i][j] = CellType::EMPTY;
		}

		CellType get(int row, int col) const {
			if (!this->isValidCell(row, col))
				return CellType::EMPTY;
			return data[row][col];
		}


	private:
		CellType data[MAPSIZE][MAPSIZE];
		// ��ப� - �������� ��ࠡ��,
		// ��ࢮ� �᫮ - ������⢮ ��ࠡ���,
		// ��஥ �᫮ - ����� ��ࠡ��
		std::vector<ShipInfo> SHIPS = {
			{"������寠�㡭��", 1, 4},
			{"���寠�㡭��", 2, 3},
			{"���寠�㡭��", 3, 2},
			{"�������㡭��", 4, 1}
		};
};

class ControlEntity {
	public:
		ControlEntity(const std::string& name) : entityName(name) {}
		virtual std::pair<int, int> getAttackCell() {};
		
		void printMaps(){
			std::cout << "[*] " << this->entityName << "\n";
			std::cout << "���� ��ࠡ���:\n";
			this->shipMap.print();
			std::cout << "�⠪������ ����:\n";
			this->attackMap.print();
			std::cout << "\n";
		}

	protected:
		Map shipMap;
		Map attackMap;

	private:
		std::string entityName;
};

class Player : public ControlEntity {
	public:
		Player() : ControlEntity("��ப") {
			if (filesystem::is_regular_file(filesystem::current_path().append("input.txt"))) {
				this->shipMap.loadFromFile(filesystem::current_path().append("input.txt").generic_string());
			}else{
				this->shipMap.loadFromConsole();
			}
		}

		std::pair<int, int> getAttackCell() override {
			std::cout << "������ ���� ��� �⠪� (\"�1\"): " ;
			std::string s;
			std::cin >> s;
			int column = int(s[0])-97;
			int row = std::stoi(s.substr(1))-1;
			if (!this->attackMap.isValidCell(row, column)) {
				std::cout << s << " " << row << " " << column << "\n";
				std::cout << "������� ������ �����!\n\t����㯭�: " << char(97) << "1-" << char(96+MAPSIZE) << MAPSIZE << "";
				return {0, 0};
			}
			return {row, column};
		}

		void applyAttack(int row, int column, Map::CellType type) {
			this->attackMap.applyAttack(row, column, type);
		}
};

class Bot : public ControlEntity {
	public:
		Bot() : ControlEntity("���") {
			this->shipMap.generateRandomMap();
		}
		std::pair<int, int> getAttackCell() override  {

		}

		Map::CellType doAttack(int row, int column) {
			switch (this->shipMap.get(row, column)){
				case Map::CellType::EMPTY:
					this->shipMap.applyAttack(row, column, Map::CellType::ATTACKED);
					return Map::CellType::ATTACKED;
					break;
				case Map::CellType::SHIP:
					this->shipMap.applyAttack(row, column, Map::CellType::ATTACKEDSHIP);
					return Map::CellType::ATTACKEDSHIP;
					break;
				case Map::CellType::ATTACKEDSHIP:
					this->shipMap.applyAttack(row, column, Map::CellType::ATTACKEDSHIP);
					return Map::CellType::ATTACKEDSHIP;
					break;
				default:
					return Map::CellType::ATTACKED;
			}
		}
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

	Bot bot;
	cout << "BotMap:\n";
	bot.printMaps();
	cout << "\n";

	Player player;
	player.printMaps();
	auto [row, column] = player.getAttackCell();
	player.applyAttack(row, column, bot.doAttack(row, column));
	player.printMaps();

	bot.printMaps();


	return 0;
}
