#include <iostream>		
#include "src/bot.hpp"
#include "src/player.hpp"


int main() {
	std::cout << "                     _______. _______      ___       " << "\n"
		 << "                    /       ||   ____|    /   \\     " << "\n"
		 << "                   |   (----`|  |__      /  ^  \\    " << "\n"
		 << "                    \\   \\    |   __|    /  /_\\  \\" << "\n"
		 << "                .----)   |   |  |____  /  _____  \\  " << "\n"
		 << "                |_______/    |_______|/__/     \\__\\" << "\n";

	std::cout << "  .______        ___      .___________..___________. __       _______   " << "\n"
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
			std::cout <<  " __   __  _______  __   __          _     _  ___   __    _  __   __   __ " << "\n"
				 <<  "|  | |  ||       ||  | |  |        | | _ | ||   | |  |  | ||  | |  | |  | " << "\n"
				 <<  "|  |_|  ||   _   ||  | |  |        | || || ||   | |   |_| ||  | |  | |  | " << "\n"
				 <<  "|       ||  | |  ||  |_|  |        |       ||   | |       ||  | |  | |  | " << "\n"
				 <<  "|_     _||  |_|  ||       |        |       ||   | |  _    ||__| |__| |__| " << "\n"
  				 <<  "  |   |  |       ||       |        |   _   ||   | | | |   | __   __   __  " << "\n"
				 <<  "  |___|  |_______||_______|        |__| |__||___| |_|  |__||__| |__| |__|" << "\n";
			break;
		}
		if (player.isAllShipsDefeated()) {
			std::cout << " __   __  _______  __   __          ___      _______  _______  _______ "<< "\n"
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
