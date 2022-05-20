#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <fstream>
#include "controlentity.hpp"

class Player : public ControlEntity {
	public:
		Player() : ControlEntity("Player") {
			std::ifstream f("map.txt");
			if (f.is_open())
				this->shipMap.loadFromFile(f);
			else
				this->shipMap.loadFromConsole();
		}

		Cell getAttackCell() const {
			std::cout << "Enter attack cell (\"a1\"): " ;
			std::string s;
			std::getline(std::cin, s);
			Cell cell = this->recognizeCellString(s);
			if (!this->attackMap.isValidCell(cell.row, cell.column)) {
				std::cout << "Incorrect cell!\n\tCorrects are: " << "a1-" << char(96+MAPSIZE) << MAPSIZE << "\n";
				return this->getAttackCell();
			}
			return {cell.row, cell.column};
		}

		Cell recognizeCellString(const std::string &s) const {
			try {
				int column = int(s[0])-97;
				int row = std::stoi(s.substr(1).data())-1;
				return {row, column};
			}catch(...) {
				return {-1, -1};
			}
		}
};
#endif