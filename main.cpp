#include <iostream>		
#include <fstream>		
#include <cmath>	
#include <random>	
#include <string>
#include <sstream>
#include <vector>
#include <map>

using namespace std;

#define MAPSIZE 10	
#define SHIPSCOUNT 10

struct ShipInfo {
	string name;
	int count;
	int length;
};

vector<ShipInfo> AVAILABLE_SHIPS {
	{"Four - deck", 1, 4},
	{"Three - deck", 2, 3},
	{"Double deck", 3, 2},
	{"Single deck", 4, 1}
};

enum CellType {
	EMPTY, 
	SHIP, 		
	ATTACKED, 	
	ATTACKEDSHIP, 
	KILLEDSHIP
};

enum ShipDirection {
	LEFT, UP, RIGHT, DOWN
};

struct ShipPart {
	ShipPart(int r, int c, CellType t) : row(r), column(c), type(t) {}

	bool inCell(int r, int c) const {
		return r == this->row && c == this->column;
	}

	float distanceTo(int r, int c) {
		auto val = sqrt((this->row-r)*(this->row-r) + (this->column-c)*(this->column-c));
		return val;
	}

	void updateState(CellType type) {
		this->type = type;
	}

	bool isAttacked() const {
		return (this->type == CellType::ATTACKEDSHIP);
	}

	int row;
	int column;
	CellType type;
};


class Ship {
	public:
		Ship(int row, int column, int length = 1, ShipDirection direction = ShipDirection::RIGHT) {
			for (int i = 0; i < length; ++i) {
				switch (direction) {
					case ShipDirection::LEFT:
						this->shipParts.emplace_back(row, column-i, CellType::SHIP);
						break;
					case ShipDirection::UP:
						this->shipParts.emplace_back(row-i, column, CellType::SHIP);
						break;
					case ShipDirection::RIGHT:
						this->shipParts.emplace_back(row, column+i, CellType::SHIP);
						break;
					case ShipDirection::DOWN: 
						this->shipParts.emplace_back(row+i, column, CellType::SHIP);
						break;
				}
			}
		}

		Ship(int row, int col, CellType type) {
			this->shipParts.emplace_back(row, col, type);
		}

		bool contains(int row, int column) const {
			for (auto &shipPart : this->shipParts) {
				if (shipPart.inCell(row, column))
					return true;
			}
			return false;
		}

		CellType partState(int row, int column) const {
			for(auto &shipPart : this->shipParts) {
				if (shipPart.column == column && shipPart.row == row)
					return shipPart.type;
			}
			throw runtime_error("Incorrect ship cell");
		}

		CellType applyAttack(int row, int column) {
			for (auto &shipPart : shipParts ) {
				if (shipPart.inCell(row, column)) {
					shipPart.updateState(CellType::ATTACKEDSHIP);
					if (this->isAllShipPartsAttacked()) {
						this->kill();
						return CellType::KILLEDSHIP;
					}
					return CellType::ATTACKEDSHIP;
				}
			}
		}

		bool isAllShipPartsAttacked() const {
			for (auto &shipPart : this->shipParts) {
				if (!shipPart.isAttacked())
					return false;
			}
			return true;
		}

		bool isKilled() const {
			for (auto &shipPart : this->shipParts) {
				if (shipPart.type != CellType::KILLEDSHIP) 
					return false;
			}
			return true;
		}

		void kill()  {
			for (auto &shipPart : shipParts )
				shipPart.updateState(CellType::KILLEDSHIP);
		}

		vector<ShipPart> parts() const {
			return this->shipParts;
		}

		float minimalDistanceTo(int row, int col) {
			float minDistance = 10;
			for (auto &shipPart : this->shipParts) {
				float distance = shipPart.distanceTo(row, col);
				if ( distance < minDistance)
					minDistance = distance;
			}
			return minDistance;
		}
	
		void addPart(int row, int column, CellType type) {
			this->shipParts.emplace_back(row, column, type);
		}


	private:
		vector<ShipPart> shipParts;
};


class Map {
	public:
		enum RecognizeError {
			NoError, IncorrectDirection, IncorrectCell, CellAlreadyUsed
		};


		CellType cellAttacked(int row, int column) {
			for (auto &ship : this->ships) {
				if (ship.contains(row, column)) {
					return ship.applyAttack(row, column);
				}
			}

			if (!this->isAlreadyInVoidAttacks(row, column))
				this->voidAttacks.emplace(row, column);
			
			return CellType::ATTACKED;
		}

		void applyAttack(int row, int column, CellType type) {
			if (type == CellType::ATTACKED) {
				if (!this->isAlreadyInVoidAttacks(row, column))
					this->voidAttacks.emplace(row, column);
			}else {
				for (auto &ship : this->ships) {
					for (auto &part : ship.parts()) {

						if ((part.row == row && abs(part.column - column) == 1) || (part.column == column && abs(part.row - row) == 1) ) {
							ship.addPart(row, column, CellType::ATTACKEDSHIP);
							if (type == CellType::KILLEDSHIP)
								ship.kill();
							return;
						}
					}
				}
				this->ships.emplace_back(row, column, type);
			}
		}

		bool isAlreadyInVoidAttacks(int row, int column) const {
			auto allValuesForGivenRow = this->voidAttacks.equal_range(row);
			for (auto it = allValuesForGivenRow.first; it != allValuesForGivenRow.second; ++it) {
				if (it->first == row && it->second == column) {
					return true;		
				}
			}
			return false;
		}

		void loadFromFile(ifstream &f) {
			if (!f.is_open())
				throw runtime_error("Can't open file");

			int lineCount = 0;
			while (f.peek() != EOF) {
				int length, row, column;
				string direction;
				f >> length >> row >> column >> direction;
				row -= 1;
				column -= 1;
				switch(this->recognizeAndSetShipSource(row, column, direction, length)) {
					case RecognizeError::NoError: 
						break; 
					case RecognizeError::IncorrectDirection:
						throw runtime_error("Error file recognizig!\nAt line:" + to_string(lineCount) + " Incorrect direction!\n");
						break;
					case RecognizeError::IncorrectCell:
						throw runtime_error("Error file recognizig!\nAt line:" + to_string(lineCount) + " Incorrect row or column!\n");
						break;
					case RecognizeError::CellAlreadyUsed:
						throw runtime_error("Error file recognizig!\nAt line:" + to_string(lineCount) + " Cell with this direction already in use. Change another!\n");
						break;
				}
				lineCount += 1;
			}
			if (this->ships.size() != SHIPSCOUNT)
				throw runtime_error("Too few ships!");
		}

		void loadFromConsole() {
			auto ship = AVAILABLE_SHIPS.begin();
			while (true) {
				if (ship == AVAILABLE_SHIPS.end())
					break;

				cout << "Arrange  " << ship->count << " " << ship->name << ". Your map:\n";
				this->print();

				cout << "Enter ship data \"ROW COLUMN DIRECTION\".\n";

				int row, col;
				string direction;
				cin >> row >> col >> direction;
				row -= 1;
				col -= 1;
				switch(this->recognizeAndSetShipSource(row, col, direction, ship->length)) {
					case RecognizeError::NoError: 
						ship->count -= 1;
						break;
					case RecognizeError::IncorrectDirection:
						cout << "Incorrect direction!\n";
						break;
					case RecognizeError::IncorrectCell:
						cout << "Incorrect row or column!\n";
						break;
					case RecognizeError::CellAlreadyUsed:
						cout << "Cell with this direction already in use. Change another!\n";
						break;
				}
				if (ship->count == 0)
					++ship;
			}
		}

		RecognizeError recognizeAndSetShipSource(int row, int column, const string &stringDirection, int length) {
			ShipDirection direction;
			if (this->isValidCell(row, column)) {
				if (stringDirection == "right") {
					direction = ShipDirection::RIGHT;
				}else if (stringDirection == "left") {
					direction = ShipDirection::LEFT;
				}else if (stringDirection == "up") {
					direction = ShipDirection::UP;
				}else if (stringDirection == "down") {
					direction = ShipDirection::DOWN;
				} else {
					return RecognizeError::IncorrectDirection;
				}
				if (this->setShipIfCanPlace(row, column, direction, length)) {
					return RecognizeError::NoError;	
				}else{
					return RecognizeError::CellAlreadyUsed;
				}
			}
			return RecognizeError::IncorrectCell;
		}

		void generateRandomMap() {
			random_device dev;
			mt19937 rng(dev());
			uniform_int_distribution<mt19937::result_type> randomMapNumber(0, MAPSIZE-1);
			uniform_int_distribution<mt19937::result_type> randomDirection(0, 3);
			for (auto &ship:AVAILABLE_SHIPS)
				for (int shipsCount = 0; shipsCount < ship.count; ++shipsCount)
					while (!this->setShipIfCanPlace(randomMapNumber(rng), randomMapNumber(rng), ShipDirection(randomDirection(rng)), ship.length));
		}

		bool setShipIfCanPlace(int row, int column, ShipDirection direction, int length) {
			if (!this->isValidCell(row, column))
				return false;

			switch (direction) {
				case ShipDirection::LEFT:
					if (column - length < 0) 
						return false;
					
					for (int i = 0; i < length; ++i)
						if (!this->canPlaceShipInCell(row, column-i))
							return false;
					break;

				case ShipDirection::UP:
					if (row - length < 0)
						return false;

					for (int i = 0; i < length; ++i)
						if (!this->canPlaceShipInCell(row-i, column))
							return false;
					break;

				case ShipDirection::RIGHT:
					if (column + length >= MAPSIZE) 
						return false;

					for (int i = 0; i < length; ++i) 
						if (!this->canPlaceShipInCell(row, column+i))
							return false;
					break;

				case ShipDirection::DOWN:
					if (row + length >= MAPSIZE)
						return false;

					for (int i = 0; i < length; ++i)
						if (!this->canPlaceShipInCell(row+i, column))
							return false;
					break;
			}
			this->ships.emplace_back(row, column, length, direction);
			return true;
		}

		bool canPlaceShipInCell(int row, int column) {
			for (auto &ship : this->ships) {
				if (ship.minimalDistanceTo(row, column) < 2)
					return false;
			}
			return true;
		}

		bool isValidCell(int row, int column) const {
			if (row < 0 || row >= MAPSIZE || column < 0 || column >= MAPSIZE)
				return false;
			return true;
		}

		void print() const {
			this->printHeader();
			cout << "\n";
			for (int i = 0; i < MAPSIZE; ++i) {
				this->printRow(i);
				cout << "\n";
			}
		}

		void printHeader() const {
			cout << "   "; 
			for (int i = 97; i < MAPSIZE+97; ++i)
				cout << char(i) << " "; 
		}

		void printRow(int row) const {
			cout << (row+1);

			if (row != 9) {
				cout << " ";
			}
			cout  <<"|";

			for (int column = 0; column < MAPSIZE; ++column){
				switch (this->get(row, column)) {
					case CellType::EMPTY: 
						cout << " ";
						break;
					case CellType::SHIP:
						cout << "#";
						break;
					case CellType::ATTACKEDSHIP:
						cout << "~";
						break;
					case CellType::ATTACKED:
						cout << "*";
						break;
					case CellType::KILLEDSHIP:
						cout << "X";
						break;
				}
				cout << "|";
			}
		}

		CellType get(int row, int column) const {
			if (!this->isValidCell(row, column))
				return CellType::EMPTY;
			
			for (auto ship : this->ships) {
				if (ship.contains(row, column)) {
					return ship.partState(row, column);
				}
			}
			
			for (auto cell : this->voidAttacks) {
				if (cell.first == row && cell.second == column)
					return CellType::ATTACKED;
			}
			return CellType::EMPTY;
		}

		bool isAllShipsDefeated() const {
			for (auto &ship : this->ships) {
				if (!ship.isKilled())
					return false;
			}
			return true;
		}

	private:
		vector<Ship> ships;
		multimap<int, int> voidAttacks;
};


class ControlEntity {
	public:
		ControlEntity(const string& name) : entityName(name) {}
		
		void printMaps(){
			cout << "[*] " << this->entityName << "\n";
			
			this->shipMap.printHeader();
			cout << "       "; 
			this->attackMap.printHeader();
			cout << "\n";

			for (int row = 0; row < MAPSIZE; ++row) {
				this->shipMap.printRow(row);
				cout << "       ";
				
				this->attackMap.printRow(row);
				cout << "\n";
			}
		}

		CellType applyAttackAtShipMap(int row, int column) {
			return this->shipMap.cellAttacked(row, column);
		}

		void applyAttackAtAttackMap(int row, int column, CellType type) {
			this->attackMap.applyAttack(row, column, type);
		}

		bool isAllShipsDefeated() const {
			return this->shipMap.isAllShipsDefeated();
		}

	protected:
		Map shipMap;
		Map attackMap;

	private:
		string entityName;
};



class Player : public ControlEntity {
	public:
		Player() : ControlEntity("Player") {
			std:ifstream f("map.txt");
			if (f.is_open())
				this->shipMap.loadFromFile(f);
			else
				this->shipMap.loadFromConsole();
		}

		pair<int, int> getAttackCell() const {
			cout << "Enter attack cell (\"a1\"): " ;
			string s;
			getline(cin, s);
			pair<int, int> cell = this->recognizeCellString(s);
			if (!this->attackMap.isValidCell(cell.first, cell.second)) {
				cout << "Incorrect cell!\n\tCorrects are: " << "a1-" << char(96+MAPSIZE) << MAPSIZE << "\n";
				return this->getAttackCell();
			}
			return {cell.first, cell.second};
		}

		pair<int, int> recognizeCellString(const std::string &s) const {
			try {
				int column = int(s[0])-97;
				int row = stoi(s.substr(1).data())-1;
				return {row, column};
			}catch(...) {
				return {-1, -1};
			}
		}
};


class Bot : public ControlEntity {
	struct AttackInfo {
		pair<int, int> toCell() const {
			return {row, column};
		}
		int row;
		int column;
	};

	public:
		Bot() : ControlEntity("Bot") {
			this->shipMap.generateRandomMap();
		}

		pair<int, int> getAttackCell() {
			auto [randomRow, randomColumn] = this->generateRandomCell();
			this->attackHistory.push_back({randomRow, randomColumn});
			return this->attackHistory.back().toCell(); 
		}

		pair<int, int> generateRandomCell() const {
			random_device dev;
			mt19937 rng(dev());
			uniform_int_distribution<mt19937::result_type> randomMapNumber(0, MAPSIZE-1);
			int row = randomMapNumber(rng);
			int column = randomMapNumber(rng);
			if (!this->isAlreadyAttacked(row, column))
				return {row, column};
			return this->generateRandomCell();
		}
		
		bool isAlreadyAttacked(int row, int column) const {
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

	Bot bot;
	Player player;

	while (true) {

		// bot.printMaps();

		player.printMaps();

		{
			auto [attackRow, attackColumn] = player.getAttackCell();
			player.applyAttackAtAttackMap(attackRow, attackColumn, bot.applyAttackAtShipMap(attackRow, attackColumn));
		}
		{
			auto [attackRow, attackColumn] = bot.getAttackCell();
			std::cout << "\nBot attack at  " << char(attackColumn+97) << attackRow+1 << "\n\n";
			bot.applyAttackAtAttackMap(attackRow, attackColumn, player.applyAttackAtShipMap(attackRow, attackColumn));
		}

	
		if (bot.isAllShipsDefeated()) {
			cout <<  " __   __  _______  __   __          _     _  ___   __    _  __   __   __ " << "\n"
				 <<  "|  | |  ||       ||  | |  |        | | _ | ||   | |  |  | ||  | |  | |  | " << "\n"
				 <<  "|  |_|  ||   _   ||  | |  |        | || || ||   | |   |_| ||  | |  | |  | " << "\n"
				 <<  "|       ||  | |  ||  |_|  |        |       ||   | |       ||  | |  | |  | " << "\n"
				 <<  "|_     _||  |_|  ||       |        |       ||   | |  _    ||__| |__| |__| " << "\n"
  				 <<  "  |   |  |       ||       |        |   _   ||   | | | |   | __   __   __  " << "\n"
				 <<  "  |___|  |_______||_______|        |__| |__||___| |_|  |__||__| |__| |__|" << "\n";
			break;
		}
		if (player.isAllShipsDefeated()) {
			cout << " __   __  _______  __   __          ___      _______  _______  _______ "<< "\n"
				 << "|  | |  ||       ||  | |  |        |   |    |       ||       ||       |"<< "\n"
				 << "|  |_|  ||   _   ||  | |  |        |   |    |   _   ||  _____||    ___|"<< "\n"
				 << "|       ||  | |  ||  |_|  |        |   |    |  | |  || |_____ |   |___ "<< "\n"
				 << "|_     _||  |_|  ||       |        |   |___ |  |_|  ||_____  ||    ___|"<< "\n"
  				 << "  |   |  |       ||       |        |       ||       | _____| ||   |___ "<< "\n"
  				 << "  |___|  |_______||_______|        |_______||_______||_______||_______|"<< "\n";
			break;
		}
	}

	return 0;
}
