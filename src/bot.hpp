#ifndef BOT_HPP
#define BOT_HPP

#include "controlentity.hpp"

class Bot : public ControlEntity {
	public:
		Bot() : ControlEntity("Bot") {
			this->shipMap.generateRandomMap();
		}

		Cell getAttackCell() {
			auto [randomRow, randomColumn] = this->generateRandomCell();
			this->attackHistory.push_back({randomRow, randomColumn});
			return this->attackHistory.back(); 
		}

		Cell generateRandomCell() const {
			std::random_device dev;
			std::mt19937 rng(dev());
			std::uniform_int_distribution<std::mt19937::result_type> randomMapNumber(0, MAPSIZE-1);
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
		std::vector<Cell> attackHistory;	
};
#endif