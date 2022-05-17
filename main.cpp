#include <iostream>			// �㦭� ��� �뢮�� � ���᮫�
#include <filesystem>		// ��� �����㦥��� 䠩�� � ���⮩
#include <fstream>			// ��� ���뢠��� ����� �� 䠩��
#include <cmath>			// ��� sqrt ( ��७� )
#include <random>			// ��� ������ �����樨 ����� � �⠪� ���
#include <string>			// ��� ࠡ��� � ��ப���
#include <vector>			// ���ᨢ ������ vector - ��� ᯨ᪠ ���ਨ �⠪
#include <map>				// ���ᨢ ������ map - ��� ᯨ᪠ ���ਨ �⠪

using namespace std;

#define MAPSIZE 10		// ������ �����
#define SHIPSCOUNT 10
		
// ��ꥪ� ��� �࠭���� ���ଠ樨 � ��ࠡ��
struct ShipInfo {
	string name;
	int count;
	int length;
};

// ����� � �ᥬ� �������묨 ��ࠡ�ﬨ
// ��ப� - �������� ��ࠡ��,
// ��ࢮ� �᫮ - ������⢮ ��ࠡ���,
// ��஥ �᫮ - ����� ��ࠡ��
vector<ShipInfo> AVAILABLE_SHIPS {
	{"������寠�㡭��", 1, 4},
	{"���寠�㡭��", 2, 3},
	{"���寠�㡭��", 3, 2},
	{"�������㡭��", 4, 1}
};

// ����� ���⪨
enum CellType {
	EMPTY,  		// �����
	SHIP, 			// � ��ࠡ���
	ATTACKED, 		// �⠪������, "����"
	ATTACKEDSHIP, 	// �⠪������, ������ � ��ࠡ��
	KILLEDSHIP		// ����� ��ࠡ��
};

// ���ࠫ���� ��ࠡ��, �㦭� ��� ���⠢����� � ��砩��� �����樨 (��� ���)
enum ShipDirection {
	LEFT, UP, RIGHT, DOWN
};

// ��ꥪ� ����� �।�⠢��� ᮡ�� ���� ����� ��ࠡ��
// ����ন� ������ � ⨯ �祩��
// ������� - �� �����, �� ����騩 �㡫��� ����� � ��६����
struct ShipPart {
	// �⠭����� ���������,  r - ��ப�, c - �⮫���, t - ⨯ ( ࠭��, 㡨�, ����)
	ShipPart(int r, int c, CellType t) : row(r), column(c), type(t) {}

	// ��⮤ ����� ShipPart, �����頥� true, �᫨ ������ ���� ��室���� � ���⪥ ( ���⪠ �������� ��㬥�⠬� )
	bool inCell(int r, int c) const {
		return r == this->row && c == this->column;
	}

	// �����뢠�� ���⠭�� �� ��㣮� ���⪨, r - ��ப�, c - �⮫���
	float distanceTo(int r, int c) {
		// ���筠� ��㫠 ������ ����� ����� ���� �窠�� �१ ����� �����
		auto val = sqrt((this->row-r)*(this->row-r) + (this->column-c)*(this->column-c));
		return val;
	}

	// ��⮤ ����� ShipPart, ��⠭�������� ᢮��⢮ (��६�����) ����� � ��।�����.
	// �ᯮ������, �᫨ �㦭� ��⠭����� �� ���� ��ࠡ�� ࠭���/㡨�
	void updateState(CellType type) {
		this->type = type;
	}

	// ��⮤ ����� ShipPart, ������� ��⨭�, �᫨ ���⪠ �뫠 �⠪�����, �� ��ࠤ�� �� 㡨�
	bool isAttacked() const {
		return (this->type == CellType::ATTACKEDSHIP);
	}

	int row;
	int column;
	CellType type;
};


// ��ꥪ� ����� �।�⠢��� ᮡ�� ��ࠡ��
// ����ন� �����(���ᨢ) �� ���⮪ (��⥩) ��ࠡ��
class Ship {
	public:
		// �⠭����� ���������, ᮧ���� ��ࠡ�� � ��砫�� � ���⪥ row, column ������ length � ���ࠢ������ direction
		Ship(int row, int column, int length = 1, ShipDirection direction = ShipDirection::RIGHT) {
			// ��室���� �� 0 �� ����� �, � ����ᨬ��� �� ���ࠢ�����, ������塞 � ����� � ���⪠�� ��ࠡ�� ����
			for (int i = 0; i < length; ++i) {
				// ����ਬ, ����� ���ࠢ�����
				switch (direction) {
					// �᫨ ���ࠢ����� ���� - � 㬥��蠥� ����� �⮫��
					case ShipDirection::LEFT:
						this->shipParts.emplace_back(row, column-i, CellType::SHIP); // ���� ������ ����������
						break;
					// �᫨ ���ࠢ����� ���� - � 㬥��蠥� ����� ��ப�
					case ShipDirection::UP:
						this->shipParts.emplace_back(row-i, column, CellType::SHIP);// ���� ������ ����������
						break;
					// �᫨ ���ࠢ����� ���� - � 㢥��稢��� ����� �⮫��
					case ShipDirection::RIGHT:
						this->shipParts.emplace_back(row, column+i, CellType::SHIP);// ���� ������ ����������
						break;
					// �᫨ ���ࠢ����� ���� - � 㢥��稢��� ����� ��ப�
					case ShipDirection::DOWN: 
						this->shipParts.emplace_back(row+i, column, CellType::SHIP);// ���� ������ ����������
						break;
				}
			}
		}

		// ���������, ᮧ���� ��ࠡ�� � ���⪥ row, column � � ��।���� ⨯��
		// ���ਬ��, �᫨ �� �⠪� �� �⠪㥬 ������� ��ࠡ�� - � ����㥬�� �⨬ ��������஬ � �ࠧ� ᮧ��� 㡨��
		Ship(int row, int col, CellType type) {
			this->shipParts.emplace_back(row, col, type);
		}

		// ��⮤ ����� Ship, ������� ��⨭�, ��ࠡ�� �ᯮ�������� �� ���⪥ row, column
		// ���� - ����.
		bool contains(int row, int column) const {
			// ��室���� �� ������ ���⪥ (���)
			for (auto &shipPart : this->shipParts) {
				// �, �᫨ ���� (���⪠) ��室���� � ���⪥ row, column
				if (shipPart.inCell(row, column))
					return true; // �����頥� ��⨭�
			}
			return false;
		}

		// �����頥� ⨯ ��� (���⪨) ��ࠡ��, ����� ��室���� � ���⪥ row, column
		CellType partState(int row, int column) const {
			for(auto &shipPart : this->shipParts) {
				if (shipPart.column == column && shipPart.row == row)
					return shipPart.type;
			}
			// �᫨ ��ࠡ�� �� ᮤ�ন� ����� {row, column}, � ����뢠�� �᪫�祭�� - � �ணࠬ�� ��-� �� ⠪.
			throw runtime_error("Incorrect ship cell");
		}

		// �ਭ����� �⠪� �� ��ࠡ�� � ���⪥ {row, column}
		CellType applyAttack(int row, int column) {
			// ��室���� �� ������ ��� ��ࠡ��
			for (auto &shipPart : shipParts ) {
				// �, �᫨ ��諨 ����, ����� ��室���� � ���⪥ {row, column}
				if (shipPart.inCell(row, column)) {
					// �� �������� ����� ��ࠡ�� �� �⠪�������
					shipPart.updateState(CellType::ATTACKEDSHIP);
					// ������, ����室��� �஢���, 㬨ࠥ� �� ��ࠡ�� ��᫥ ���������
					if (this->isAllShipPartsAttacked()) {
						// �᫨ �� ��� ��ࠡ�� �뫨 �⠪����� - "㡨����" ��ࠡ��
						this->kill(); // �� � ���� ��⠭�������� ⨯ � ��� ���⮪ ��ࠡ�� � KILLEDSHIP
						// �����頥� ⨯, �᫨ ��ࠡ�� 㡨� - � KILLEDSHIP
						return CellType::KILLEDSHIP;
					}
					// �᫨ �� �� ���⪨ ��ࠡ�� �뫨 �⠪����� - � �����頥� ATTACKEDSHIP
					return CellType::ATTACKEDSHIP;
				}
			}
		}

		// ��⮤ �����, ����� �஢����, �� �� ��� ��ࠡ�� �뫨 �⠪�����
		bool isAllShipPartsAttacked() const {
			for (auto &shipPart : this->shipParts) {
				// �᫨ �� �६� ��ॡ�� ����砥� �� �⠪������� ���� - � �����頥� ����
				if (!shipPart.isAttacked())
					return false;
			}
			return true;
		}

		// ��⮤ �����, ����� �஢����, �� �� 㡨� ��ࠡ��
		bool isKilled() const {
			for (auto &shipPart : this->shipParts) {
				// �᫨ �� �६� ��ॡ�� ����砥� �� 㡨��� ���� - � �����頥� ����
				if (shipPart.type != CellType::KILLEDSHIP) 
					return false;
			}
			return true;
		}

		// ��⮤ �����, ����� 㡨���� ⥪�騩 ��ࠡ��
		void kill()  {
			// ��室���� �� ������ ��� � ��⠭�������� ⨯ � 㡨��
			for (auto &shipPart : shipParts )
				shipPart.updateState(CellType::KILLEDSHIP);
		}

		// �����頥� �� ��� ⥪�饣� ��ࠡ��
		vector<ShipPart> parts() const {
			return this->shipParts;
		}

		// ������뢠�� ���������� ���⠭�� �� ��㣮�� ��ࠡ��
		float minimalDistanceTo(int row, int col) {
			// ���� ��室���� �� ������ ��� ��ࠡ�� � �饬 ���������� ���⠭��.
			float minDistance = 10;
			for (auto &shipPart : this->shipParts) {
				// ������뢠�� ���⠭�� �� ⥪�饩 �窨 {row, col} �� ��ॡ�ࠥ��� ��� ��ࠡ��
				float distance = shipPart.distanceTo(row, col);
				if ( distance < minDistance)
					minDistance = distance;
			}
			return minDistance;
		}
	
		// ��⮤ ��� ���������� ��ࠡ�� ����� ����. ����室��� ��� ����� � �⠪�����묨 ��ࠡ�ﬨ.
		void addPart(int row, int column, CellType type) {
			this->shipParts.emplace_back(row, column, type);
		}


	private:
		// ����⢥��� ᠬ �����(���ᨢ) � ���ﬨ ��ࠡ��
		vector<ShipPart> shipParts;
};


// ��ꥪ� ����� �।�⠢��� ᮡ�� �����
// ����ন� �����(���ᨢ) �� ��ࠡ���
class Map {
	public:
		// Enum - ����᫥���, ����� ����᫥��� - ���� �᫮, ���ਬ�� NoError - �᫮ 0, IncorrectDirection - 1.
		// ����室��� ��� �ᯮ�������� �訡��
		enum RecognizeError {
			NoError, IncorrectDirection, IncorrectCell, CellAlreadyUsed
		};


		// ��⮤ ��ࠡ��뢠�� �⠪� �� ��� � ����� {row, column}
		CellType cellAttacked(int row, int column) {
			// ��室���� �� �ᥬ ��ࠡ�� � �饬 ��, ���� ���ண� ��室���� � �������� ���⪥
			for (auto &ship : this->ships) {
				if (ship.contains(row, column)) {
					// �᫨ ��室��, � �ਭ����� �⠪� �� ��ࠡ�� � �����頥� �⢥� (࠭��, 㡨�)
					return ship.applyAttack(row, column);
				}
			}
			// �᫨ �� ��室��, � �஢��塞, �������� �� ���⪠ 㦥 �뫠 �⠪�����
			// �� �஢�ઠ �㦭� ��� ⮣�, �⮡� � ��襬 ᫮��� (����� �� ��� ����:���祭��) �� �뫮 �㡫���⮢
			// ��� ������, �� �� ���ਬ�� ���⪠ {1, 0} �� �� ࠧ �� ���ᨫ��� � ᫮����
			if (!this->isAlreadyInVoidAttacks(row, column))
				// �᫨ �� �� ��� �� ���ᨫ� � ��� ᫮���� ������ �����, � ���ᨬ:
				this->voidAttacks.insert({row, column});
			
			// ����⢥��� ����� �� �������� ���⪠ �뫠 ���� -> ⥯��� ��� �⠪����� (����)
			return CellType::ATTACKED;
		}

		// ��⮤ ��ࠡ��뢠�� ���� �⠪� � ����� {row, column} � १���⮬ type
		void applyAttack(int row, int column, CellType type) {
			// �᫨ १���� - ������, �� ����, �
			if (type == CellType::ATTACKED) {
				// ��⠫��� �஢����, �� ������﫨 �� �� 㦥 ��� ����� � ���祭� �⠪�������
				if (!this->isAlreadyInVoidAttacks(row, column))
					// �᫨ �� ������﫨 - � ������塞
					this->voidAttacks.emplace(row, column);
			}else {
				// ����, ࠧ �� ��� - � ��ࠡ�� ���� 㡨� ���� ࠭��
				// ��ॡ�ࠥ� �� ��ࠡ��
				for (auto &ship : this->ships) {
					// ��ॡ�ࠥ� �� ��� ��ࠡ��
					for (auto &part : ship.parts()) {
						// �饬, �� ��室���� �� �祩�� {row, column} ����� � 㦥 �⠪������ ��ࠡ���. 
						// ��� ������, �� ���� �⠪ ����砫쭮 �� �� ����� �ᯮ������� ��ࠡ��� �� ���� � ��㣮�� ��ப�
						// ���⮬�, �᫨ ��ࠡ�� �� ࠭��/㡨� �� �⮣�, � �饬, ���� �� ���⪠ {row, column} ����� 㦥 �⠪�������� ��ࠡ��
						if ((part.row == row && abs(part.column - column) == 1) || (part.column == column && abs(part.row - row) == 1) ) {
							// �᫨ ����, � ������塞 ��� � 㦥 �������饬�
							ship.addPart(row, column, CellType::ATTACKEDSHIP);
							// �����, �᫨ �� १���⮬ �⠪� - KILLEDSHIP ( � ���� �� �뫠 ��᫥���� ����� ���� ��ࠡ��)
							if (type == CellType::KILLEDSHIP)
								// �� ����砥� ���� ��ࠡ�� ��� �����
								ship.kill();
							// return �㦥�, �⮡� �ᯮ������ �ணࠬ�� �� ��諮 ����� - ��ࠡ���� �� 㦥 ��������, ᮧ������ ���� �� �㦭�
							return;
						}
					}
				}
				// ��� �� �᪠��, ���� �� ���� ��ࠡ�� �� ���⪨ {row, column} ����� ��㣮��, �⠪�������� �� �⮣� ��ࠡ��
				// �᫨ ⠪��� ��ࠡ�� ��� (�� ��� �� ���� ���� ࠧ)
				// �� ���� ������塞 ���� ������� ��ࠡ�� � ��� ᯨ�祪.
				this->ships.emplace_back(row, column, type);
			}
		}

		// ��⮤ ��� �஢�ન, ���� �� �� 㦥 � ��� ���.
		bool isAlreadyInVoidAttacks(int row, int column) const {
			// �饬 ����� �⮫�殢 ���  �⠪������� �祥�, ��ப� ������ ࠢ�� row
			auto allValuesForGivenRow = this->voidAttacks.equal_range(row);
			// ������, �������� �� �ᥬ ⠪�� ����⠬
			for (auto it = allValuesForGivenRow.first; it != allValuesForGivenRow.second; ++it) {
				// �஢��塞, ᮤ�ন��� �� � ���筥 �⠪������� ���⮪ ��� ���⪠ {row, column}
				if (it->first == row && it->second == column) {
					// �᫨ ᮤ�ন��� - �����頥� ��⨭�
					return true;		
				}
			}
			// ���� - ����
			return false;
		}

		// ��⮤, ����� ����㦠�� ����� �� 䠩��, ��� 䠩�� - ��।����� � ��६����� filename
		void loadFromFile(const string &filename) {
			// ifstream - ��ꥪ�, ����� �������� ࠡ���� � 䠩����.
			ifstream f;
			// ���뢠�� ��� 䠩���
			f.open(filename);
			// �᫨ �� ����稫��� - ����뢠�� �᪫�祭�� ( �ணࠬ�� ���㡠���� )
			if (!f.is_open())
				throw runtime_error("Can't open" + filename);

			// ����稪 ��ப �㦥�, �⮡� �᫨ ���� �訡�� �� �⥭�� 䠩�� - ����� �� ����� ��ப� �� �訡��. 
			int lineCount = 0;
			// peek() - �����頥�, �� �� ���뢠�� �� ���� ᫥���騩 ᨬ���
			while (f.peek() != EOF) {
				// ������ ��६���� �⮡� � ��� ����� �뫮 ����� ���� �� 䠩��
				int length, row, column;
				string direction;
				// ���뢠�� ����� � ��६����
				f >> length >> row >> column >> direction;
				// ���⠥� �������, ��⮬� �� �㬥��� � ���ᨢ�� ��稭����� � 0, � � ���᪮� ��� - � 1.
				row -= 1;
				column -= 1;
				// ��⠥��� �ᯮ����� ���ࠢ����� �, �᫨ �������� - ᮧ���� ��ࠡ��.
				switch(this->recognizeAndSetShipSource(row, column, direction, length)) {
					// ��� ��� �� ��㪨 � �㦭� enum ( ����᫥��� ) - ⠪ 㤮���� �ᯮ�������, ����� �뫨 �訡��
					case RecognizeError::NoError: // ��� ���, �訡�� ���
						break; // ��室��
					case RecognizeError::IncorrectDirection: // ��-��-��! �� ����稫��� �ᯮ���� ���ࠢ�����
						throw runtime_error("�訡�� �ᯮ�������� 䠩��!\n��ப�:" + to_string(lineCount) + " ����୮� ���ࠢ�����! ��������: \"�����\", \"��ࠢ�\", \"�����\", \"����\" \n");
						break;
					case RecognizeError::IncorrectCell:// �訡�� - ���ࠢ��쭠� ��ப�/�⮫���
						throw runtime_error("�訡�� �ᯮ�������� 䠩��!\n��ப�:" + to_string(lineCount) + " ����ୠ� ��ப� ���/� �⮫���!\n");
						break;
					case RecognizeError::CellAlreadyUsed: // �訡�� - � �⮩ ���⪥ ����� ࠧ������ ��ࠡ��, �.�. �冷� ��室���� ��㣮�!
						throw runtime_error("�訡�� �ᯮ�������� 䠩��!\n��ப�:" + to_string(lineCount) + " ���⪠ � �⨬ ���ࠢ������ 㦥 �����!\n");
						break;
				}
				// �᫨ �ணࠬ�� �� �맢��� �� ���� throw ( � ⮣�� ��� �४��� �믮������ � ��譥��� � �訡��� ), � 㢥��稢��� ����稪 ��ப, ��� ��� �ᯮ�������
				lineCount += 1;
			}
			// �᫨ �� 䠩�� ��⠫��� �������筮 ��ࠡ��� - ��뢠�� �訡��.
			if (this->ships.size() != SHIPSCOUNT)
				throw runtime_error("���� ��ࠡ���!");
		}

		// ��⮤, ����� ����㦠�� ����� �� ���᮫� (����訢��� ����� � ���짮��⥫�)
		void loadFromConsole() {
			// �⮡� �� ��������� � ������⢮� ��ࠡ���, � ��� ���� �� ����� ����砫�� ᯨ᮪ (AVAILABLE_SHIPS)
			//  ���⮬� �㤥� �� ���� ���஢����� (.begin() - �����頥� ����� �� ���� ������, .end() - ����� �� ����� ��᫥ ��᫥�����).
			// ��१ ++<�����> - ����� ���室��� � ᫥���饬� ������
			auto ship = AVAILABLE_SHIPS.begin();
			// ���� ��᪮����, ��⮬� ��, �᫨ ���짮��⥫� ������ ��-� ���ࠢ��쭮, 
			// ����� �㤥� ���� ��� ࠧ �������� ��� �� 横�, �� 㢥��稢�� �����.
			while (true) {
				// �᫨ ����� ���⨣ ���� - � ���뢠�� 横�
				if (ship == AVAILABLE_SHIPS.end())
					break;

				cout << "����⠢�� " << ship->count << " " << ship->name << ". ��� ����:\n";
				// print - ���⠥� ⥪���� ����� ��ࠡ��� (�⮡� ����� �뫮 �ਥ��஢�����, �㤠 �⠢���)
				this->print();

				// ����訢��� � ���짮��⥫� ����� � ��ࠡ��
				cout << "������ ����� ��ࠡ�� � ���� \"������ ������� �����������\". ���ਬ��: \"1 1 ��ࠢ�\"\n";

				// ��� � � ���뢠���� �� 䠩�� - ᮧ��� ��६���� ��� ��⢠��� � ���
				int row, col;
				string direction;
				// ���뢠�� ����� � ��६����
				cin >> row >> col >> direction;
				// ���⠥� �������, ��⮬� �� �㬥��� � ���ᨢ�� ��稭����� � 0, � � ���᪮� ��� - � 1.
				row -= 1;
				col -= 1;
				// ��⠥��� �ᯮ����� ���ࠢ����� �, �᫨ �������� - ᮧ���� ��ࠡ��.
				switch(this->recognizeAndSetShipSource(row, col, direction, ship->length)) {
					case RecognizeError::NoError: // ��� ���, �訡�� ���
						// ��� �訡�� ��� - 㬥��蠥� ������⢮ ��ࠡ���
						ship->count -= 1;
						break;
					case RecognizeError::IncorrectDirection: // ��-��-��! �� ����稫��� �ᯮ���� ���ࠢ�����
						cout << "����୮� ���ࠢ�����! ��������: \"�����\", \"��ࠢ�\", \"�����\", \"����\" \n";
						break;
					case RecognizeError::IncorrectCell:// �訡�� - ���ࠢ��쭠� ��ப�/�⮫���
						cout << "����ୠ� ��ப� ���/� �⮫���!\n";
						break;
					case RecognizeError::CellAlreadyUsed:// �訡�� - � �⮩ ���⪥ ����� ࠧ������ ��ࠡ��, �.�. �冷� ��室���� ��㣮�!
						cout << "���⪠ � �⨬ ���ࠢ������ 㦥 �����!\n";
						break;
				}
				// �᫨ �ணࠬ�� �� �맢��� �� ���� throw ( � ⮣�� ��� �४��� �믮������ � ��譥��� � �訡��� ), 
				// � �஢��塞, �� �� �� ��ࠡ�� ������� ⨯� �� ���⠢���
				if (ship->count == 0)
					++ship;  // ship - �����, ���⮬� � ������� ++ ���室�� � ᫥���饬� ������
			}
		}

		// ��⮤ ����� Map, �ᯮ����� ������ ����� ��ࠡ�� �, �᫨ ��� ��୮ - ᮧ���� ��ࠡ��
		RecognizeError recognizeAndSetShipSource(int row, int column, const string &stringDirection, int length) {
			// � ��� ��६����� ������� �ᯮ������� ���祭�� ���ࠢ�����
			// ��� ������, ����� �� ����� ⮫쪮 ��ப�, � ��� 㤮��⢠ - ��� �㦭� ����祭�� ����᫥��� ShipDirection.
			ShipDirection direction;
			// ��ࢠ� �஢�ઠ - �������� �� �� �祩��
			if (this->isValidCell(row, column)) {
				// ����⢥��� ᠬ ����� �ᯮ������, �१ if 㧭��, ����� ���ࠢ����� � ��� � ��६����� direction
				if (stringDirection == "��ࠢ�" || stringDirection == "�ࠢ�") {
					direction = ShipDirection::RIGHT;
				}else if (stringDirection == "�����" || stringDirection == "����") {
					direction = ShipDirection::LEFT;
				}else if (stringDirection == "�����" || stringDirection == "����") {
					direction = ShipDirection::UP;
				}else if (stringDirection == "����" || stringDirection == "���") {
					direction = ShipDirection::DOWN;
				} else {
					// �᫨ �� ���� if �� �ࠡ�⠥�, �����頥� १����, �� ���ࠢ����� ����୮� 
					return RecognizeError::IncorrectDirection;
				}
				// �᫨ �� ��諨 �� �⮣� ������ - �ᯥ譮 �ᯮ����� direction!
				// ��뢠�� ��㣮� ��⮤, ����� ��⠭�������� ��ࠡ�� � �祩�� {row, column} � ������ length � ���ࠢ������ direction,
				// �᫨ �祩�� ᢮�����. ��⮤ setShipIfCanPlace �����頥� true, �᫨ ��ࠡ�� �ᯥ譮 ��⠭�����
				if (this->setShipIfCanPlace(row, column, direction, length)) {
					return RecognizeError::NoError;	// ��� �� ��� - �㭪�� ���㫠 true - �����頥� RecognizeError::NoError
				}else{
					// �᫨ �� ����� - �㪭�� ���㫠 false, ����� �祩�� 㦥 �����((
					return RecognizeError::CellAlreadyUsed;
				}
			}
			// �᫨� �祩�� ��������� - �����頥� ᮮ⢥��⢥���� ���祭�� ����᫥���
			return RecognizeError::IncorrectCell;
		}

		// �������� ࠭����� ��ࠡ�� ��� ���
		void generateRandomMap() {
			// ������� ࠭������ �ᥫ
			random_device dev;
			mt19937 rng(dev());
			// ��ꥪ� ��� �����樨 ࠭������ �ᥫ
			uniform_int_distribution<mt19937::result_type> randomMapNumber(0, MAPSIZE-1);
			uniform_int_distribution<mt19937::result_type> randomDirection(0, 3);
			// ������⭮ ��室���� �� �ᥬ �������� (����室��� ��� �����樨) ��ࠡ��
			for (auto &ship:AVAILABLE_SHIPS)
				// ����㥬�� �� �������� ��ࠡ���
				for (int shipsCount = 0; shipsCount < ship.count; ++shipsCount)
					// � ��⠥��� ���⠢��� ��ࠡ�� � ��砩�� ᣥ���஢����� ����� � ��砩�� ���ࠢ������
					// ����� �ᯮ������ while, ��⮬� �� setShipIfCanPlace �����頥� false, �᫨ �� ����稫��� 
					// ��⠭������ ��ࠡ��. ��� ������, ��ப� ���� ��������� �� �� ���, ���� ��砩�� ᣥ���஢���� ��ࠡ�� �� ��������� �� ����
					while (!this->setShipIfCanPlace(randomMapNumber(rng), randomMapNumber(rng), ShipDirection(randomDirection(rng)), ship.length));
		}

		// ��⠭�������� ��ࠡ�� � row, col � ���ࠢ����� direction � ������ length, �᫨ ��� ���⪨ �� ������
		bool setShipIfCanPlace(int row, int column, ShipDirection direction, int length) {
			// ���� �����,  �஢��塞 �⮡� ��砫� ��ࠡ�� (��砫쭠� ���⪠) �뫠 ��������
			if (!this->isValidCell(row, column))
				return false;

			// ������, � ����ᨬ��� �� ���ࠢ�����, �஢��塞 �� ��ࠡ�� ��������� �� ����
			switch (direction) {
				case ShipDirection::LEFT:
					if (column - length < 0) // �᫨ ��ࠡ�� ���ࠢ��� �����, � ��� ����� ������ ���� ����� ����� �࠭��� �����
						return false;
					
					// �த���� �� ����� � �஢��塞, �� �� ��� ��ࠡ�� �� ����� ��⠭�����
					for (int i = 0; i < length; ++i) // �.�. ���ࠢ����� �����, � 㬥��蠥� �⮫���
						if (!this->canPlaceShipInCell(row, column-i))
							return false;
					break;

				case ShipDirection::UP:
					if (row - length < 0) // �᫨ ��ࠡ�� ���ࠢ��� �����, � ��� ����� ������ ���� ����� ���孥� �࠭��� �����
						return false;

					// �த���� �� ����� � �஢��塞, �� �� ��� ��ࠡ�� �� ����� ��⠭�����
					for (int i = 0; i < length; ++i) // �.�. ���ࠢ����� �����, � 㬥��蠥� ��ப�
						if (!this->canPlaceShipInCell(row-i, column))
							return false;
					break;

				case ShipDirection::RIGHT:
					if (column + length >= MAPSIZE) // �᫨ ��ࠡ�� ���ࠢ��� ��ࠢ�, � ��� ����� ������ ���� ����� �ࠢ�� �࠭��� �����
						return false;

					// �த���� �� ����� � �஢��塞, �� �� ��� ��ࠡ�� �� ����� ��⠭�����
					for (int i = 0; i < length; ++i) // �.�. ���ࠢ����� ��ࠢ�, � 㢥��稢��� �⮫���
						if (!this->canPlaceShipInCell(row, column+i))
							return false;
					break;

				case ShipDirection::DOWN:
					if (row + length >= MAPSIZE) // �᫨ ��ࠡ�� ���ࠢ��� ����, � ��� ����� ������ ���� ����� ������ �࠭��� �����
						return false;

					// �த���� �� ����� � �஢��塞, �� �� ��� ��ࠡ�� �� ����� ��⠭�����
					for (int i = 0; i < length; ++i)// �.�. ���ࠢ����� ����, � 㢥��稢��� ��ப�
						if (!this->canPlaceShipInCell(row+i, column))
							return false;
					break;
			}
			// ��� �ணࠬ�� �믮������� �� �⮣� ������, ����� ��� �� � ��ࠡ�� ����� ��⠭�����.
			// ����⢥��� �� �� � ᤥ���� - ������� � ����� (���ᨢ) � ��ࠡ�ﬨ ���� ��ࠡ��:
			this->ships.push_back({row, column, length, direction});
			return true;
		}

		// �����頥� true, �᫨ �� ���� 㦥 ��⠭������� ��ࠡ�� �� ��蠥� ��⠭����� ���� ������� ��ࠡ�� � ����� {row, column}
		bool canPlaceShipInCell(int row, int column) {
			// ��室���� �� ������ ��ࠡ��
			for (auto &ship : this->ships) {
				// � ᬮ�ਬ ����� �� �������� ���⪨ {row, column} �� ⥪�饣� ��ࠡ�� ship
				// ��� ������ ���� ����� ��� ࠢ�� 2, ��⮬� �� ������ ⠪�� ����ﭨ� �������쭮 ����� ���� ��ࠡ�ﬨ
				if (ship.minimalDistanceTo(row, column) < 2)
					return false;
			}
			// ��� �ணࠬ�� ���諠 �� � - ��� ��, �����頥� ��⨭�, ��ࠡ�� ����� ࠧ������
			return true;
		}

		// �஢���� �祩�� {row, column} �� ����������
		bool isValidCell(int row, int column) const {
			// �祩�� �������, �᫨ {row, column} ����� ��� ࠢ�� ��� � ����� ࠧ��� �����
			if (row < 0 || row >= MAPSIZE || column < 0 || column >= MAPSIZE)
				return false;
			return true;
		}

		// �뢮��� �� �࠭ ���� �����
		void print() const {
			// �뢮��� ��������� ����� (a b c d, ...)
			this->printHeader();
			cout << "\n";
			// �뢮��� �᭮���� �����
			for (int i = 0; i < MAPSIZE; ++i) {
				// �뢮��� �� ���᮫� i-� ��ப�
				this->printRow(i);
				cout << "\n";
			}
		}

		// �뢮��� �� �࠭ ��������� �����
		void printHeader() const {
			cout << "   "; // ��ᨢ� �����
			// 97 - ����� ᨬ���� a � ����஢�� ASCII
			for (int i = 97; i < MAPSIZE+97; ++i)
				cout << char(i) << " "; // �ॢ��� �᫮ � ᨬ���, �뢮��� ��������� ����� 
		}

		// �뢮��� �� �࠭ ��ப� �����
		void printRow(int row) const {
			// �.�. � ��� ����� ��稭����� � 0, � � ���� - ������ ���� � �������, �뢮��� row+1
			cout << (row+1);

			// �᫨ �� ���⠥� �� 10-� ��ப� (9-� �᫨ ����� ��� ��襩 ���樨 � 0), 
			// � ���� �뢥�� ���. �����
			// 9_ <- "_" - �� �����
			// 10
			if (row != 9) {
				cout << " ";
			}
			cout  <<"|";

			// ��� ��� �� ��室���� �� ������� �⮫���, � �뢮��� ᨬ��� � ����ᨬ��� �� ⨯� ���⪨
			for (int column = 0; column < MAPSIZE; ++column){
				// get(row, column) - �����頥� ⨯ ���⪨ {row, column} (�⠪�����, �����, ����, ...)
				switch (this->get(row, column)) {
					case CellType::EMPTY: // ���⪠ �����
						cout << " ";
						break;
					case CellType::SHIP: // � ���⪥ ��室���� ��ࠡ��
						cout << "#";
						break;
					case CellType::ATTACKEDSHIP: //  � ���⪥ ��室���� �⠪������ ��ࠡ�� 
						cout << "X";
						break;
					case CellType::ATTACKED:  // ���⪠ �⠪�����, �� ��ࠡ�� ����� (����)
						cout << "*";
						break;
					case CellType::KILLEDSHIP: //  � ���⪥ ��室���� 㡨�� ��ࠡ�� 
						cout << "�";
						break;
				}
				cout << "|";
			}
		}

		// �����頥� ⨯ ���⪨ {row, column} (�⠪�����, �����, � ��ࠡ��, ...)
		CellType get(int row, int column) const {
			// ���� ����� �஢��塞, �� ���⪠ �������
			if (!this->isValidCell(row, column))
				// �᫨ �� �������, � �����頥� ������ ()
				return CellType::EMPTY;
			
			// ������, �᫨ ���⪠ �������, � ��室���� �� �ᥬ ��ࠡ�� � �஢��塞
			for (auto ship : this->ships) {
				// ��室���� �� �����-����� ���� ��ࠡ�� � �⮩ ���⪥
				if (ship.contains(row, column)) {
					// �᫨ ��諨 ��ࠡ�� - � �����頥� ���ﭨ� ��� ��ࠡ��, ����� ��室���� � ����訢����� ���⪥ {row, column}
					return ship.partState(row, column);
				}
			}
			
			// ��� �ᯮ������ ���� �� � - ���⪠ �������, �� ��ࠡ�� � ��� ����
			// ����� ��室���� �� �⠪������ ���⪠� � �஢��塞, ����� �� �⠪�����
			for (auto cell : this->voidAttacks) {
				// �᫨ ����⢨⥫쭮 ���⪠ �뫠 �⠪����� - �����頥� ᮮ⢥�����饥 ���ﭨ� ���⪨(����)
				if (cell.first == row && cell.second == column)
					return CellType::ATTACKED;
			}
			// ��� �믮������ ��諮 �� �, � �� ����� ���⪠, � ��� ��� ��ࠡ�� � ��� �� �뫠 �⠪�����
			return CellType::EMPTY;
		}

		// �஢����, �� �� ��ࠡ�� �뫨 ���������
		bool isAllShipsDefeated() const {
			// ���� ��室���� �� �ᥬ ��ࠡ��
			for (auto &ship : this->ships) {
				// � �஢��塞, 㡨�� �� ���. �᫨ ��室�� ��� �� ���� �� 㡨��
				if (!ship.isKilled())
					// �����頥� ����
					return false;
			}
			return true;
		}

	private:
		// ����� (���ᨢ) � ��ࠡ�ﬨ
		vector<Ship> ships;
		// ����� ��� ����:���祭��, �࠭�� �� ���⪨ �� ����� �� ��५﫨 � �� ������
		// multimap �⫨砥��� �� map ⥬, �� ��� ������ ���� ����� �࠭��� ��᪮�쪮 ���祭��
		multimap<int, int> voidAttacks;
};



// ��ꥪ� ����� �।�⠢��� ᮡ�� ������ ����� ��� ��ப� � ���
// ����ন� ������� ������, ����� ���������� � � ��� � � ��ப�
class ControlEntity {
	public:
		// ���������, ����� ��࠭�� ��� ��ப� � ���� entityName
		// �� ��� �ᯮ������ �� �뢮�� �����
		ControlEntity(const string& name) : entityName(name) {}
		
		// �뢮��� �� �࠭ ��� ����� (���� ��ࠡ��� � ����� ���� �⠪)
		void printMaps(){
			// ��த�� ���
			cout << "[*] " << this->entityName << "\n";
			
			// �뢮��� ��������� ��� ����� ��ࠡ���
			this->shipMap.printHeader();
			cout << "       ";  // ��ᨢ� ����� ����� ���⠬�

			// �뢮��� ��������� ��� ����� �⠪
			this->attackMap.printHeader();
			cout << "\n";

			// �뢮��� �����
			// ��室� �� ������ ��ப� � ���� �����,
			for (int row = 0; row < MAPSIZE; ++row) {
				// ���ࢠ �뢮��� ᮮ⢥�������� ��ப� � ���⮩ ��ࠡ���
				this->shipMap.printRow(row);
				cout << "       ";// ��ᨢ� ����� ����� ���⠬�
				
				// ��⥬ ᮮ⢥�������� ��ப� ����� � �⠪���
				this->attackMap.printRow(row);
				cout << "\n"; // ���室�� �� ����� �����
			}
		}

		// ��⮤ ��ࠡ��뢠�� �⠪� �� ��� � ����� {row, column}
		CellType applyAttackAtShipMap(int row, int column) {
			// �맢력��� ᮮ⢥�����騩 ��⮤, ����� 㡨����/࠭��/����砥� ����� �� ���� ��ࠡ��� � �����頥� १����, ����� ���⪠ ⥯���
			// �����⨬, �᫨ ��뢠�� ��� ��⮤ ��� ���⮩ ���⪨, � ��� �⠭�� ����祭�� ��� ATTACKED, � ���� �⠪����� ����
			// �᫨ ��� ��� �� ��᫥���� ����� ���⪨ ��ࠡ��, � �⠪������� ���� ��ࠡ�� �⠭�� ����祭�� ��� ATTACKEDSHIP, � ���� ࠭���
			// �᫨ ��� ��᫥���� ����� ���⪨ ��ࠡ��, � �⠪������� ��ࠡ�� �⠭�� ����祭� ��� KILLEDSHIP, � ���� 㡨�
			return this->shipMap.cellAttacked(row, column);
		}

		// ��⮤ ��ࠡ��뢠�� ���� �⠪� � ����� {row, column}. type - १���� �⠪�, ������, ����, 㡨��, ...
		void applyAttackAtAttackMap(int row, int column, CellType type) {
			// �맢력��� ᮮ⢥�����騩 ��⮤, ����� 㡨����/࠭��/����砥� ����� �� ���� �⠪
			this->attackMap.applyAttack(row, column, type);
		}

		// �஢����, �� �� ��ࠡ�� �뫨 ���������
		bool isAllShipsDefeated() const {
			// ���� ��뢠�� ᮮ⢥�����騩 ��⮤ � ��襩 �����
			return this->shipMap.isAllShipsDefeated();
		}

	protected:
		// ���� � ��ࠡ�ﬨ
		Map shipMap;
		// ���� � �⠪���
		Map attackMap;

	private:
		// ��� ��ப�
		string entityName;
};



// �⮡� ����室���� ���� �㭪樮���쭮�� ��४�祢��� � ����� ��ப� - ��᫥�㥬��
class Player : public ControlEntity {
	public:
		// ��⠭�������� ��� ��ப� ��� "��ப"
		Player() : ControlEntity("��ப") {
			// � �஢��塞, ���� �� 䠩� map.txt
			if (filesystem::is_regular_file(filesystem::current_path().append("map.txt"))) {
				// �᫨ ���� - ���뢠�� ����� ��ࠡ��� �� ���
				this->shipMap.loadFromFile(filesystem::current_path().append("map.txt").generic_string());
			}else{
				// � ��⨢��� ��砥 ����訢��� � ���짮��⥫� �� ���᮫�
				this->shipMap.loadFromConsole();
			}
		}

		// ��⮤, ����� ����訢��� � ���짮��⥫� �祩�� ��� �⠪�
		pair<int, int> getAttackCell() const {
			cout << "������ ���� ��� �⠪� (\"�1\"): " ;
			// ���뢠�� ��� ��ப� � ��६����� s
			string s;
			getline(cin, s);
			// � ��⠥��� �ᯮ�����, � ����� �祩�� ��ப ��� ����
			auto [row, column] = this->recognizeCellString(s);
			// �᫨ �祩�� �� �������
			if (!this->attackMap.isValidCell(row, column)) {
				// � �뢮��� �訡�� � ����訢��� ����� ��� �⠪� ��� ࠧ.
				// ��� ��ᠫ ���, 96 - ����� �㪢� "a" � ASCII-����஢��. ���⢥��⢥���
				// 96+MAPSIZE - ����� ��᫥����� �⮫�� (��� MAPSIZE=10 - 106 - ����� �㪢� "j")
				cout << "������� ������ �����!\n\t����㯭�: " << "a1-" << char(96+MAPSIZE) << MAPSIZE << "\n";
				return this->getAttackCell();
			}
			// � ��⨢��� ��砥 ���� �����頥� ��
			return {row, column};
		}

		// ��⮤ ��� �ᯮ�������� ��ப� � �⮫��, ������� ���짮��⥫��
		pair<int, int> recognizeCellString(std::string_view s) const {
			try {
				// �஡㥬 �८�ࠧ����� �㪢� ���⭮ � �᫮
				int column = int(s[0])-97;
				// � ���� �८�ࠧ����� ��ப��� �।�⠢����� �᫮ � ⨯ int
				int row = stoi(s.substr(1).data())-1;
				// �᫨ ��� �� - � �����頥� �ᯮ������ ��ப� � �⮫���
				return {row, column};
			// ���� catch �믮������ ⮫쪮 �᫨ �맮������ �����-���� �᪫�祭�� (���ਬ�� ���짮��⥫� ��� ��ப� � �⮫��� � ���ࠢ��쭮� �ଠ� "1a")
			}catch(...) {
				// �����頥� �����४�� ����� �⮫�� � ��ப�
				return {-1, -1};
			}
		}
};




// �⮡� ����室���� ���� �㭪樮���쭮�� ��४�祢��� � ������ ��� - ��᫥�㥬��
class Bot : public ControlEntity {
	
	// ������ઠ ����� ᮤ�ন� ��ப� � �⮫���, �᮫����� ��� ���ਨ �⠪
	struct AttackInfo {
		pair<int, int> cell() const {
			return {row, column};
		}
		int row;
		int column;
	};

	public:
		// ��⠭�������� ��� ��ப� ��� "���"
		Bot() : ControlEntity("���") {
			// �������� ࠭������ ����� � ��ࠡ�ﬨ
			this->shipMap.generateRandomMap();
		}

		// ��⮤, �����頥� �祩��, � ������ ��� ��� �⠪����� 
		pair<int, int> getAttackCell() {
			// ������㥬 ��砩��� �祩�� 
			auto [row, column] = this->generateRandomCell();
			// ������塞 �� � ����� �⠪
			this->attackHistory.push_back({row, column});
			return this->attackHistory.back().cell(); 
		}

		// ��⮤, �������� ��砩��� �祩��
		pair<int, int> generateRandomCell() const {
			// ���樠�����㥬 ������� ��砩��� �ᥫ
			random_device dev;
			mt19937 rng(dev());
			uniform_int_distribution<mt19937::result_type> randomMapNumber(0, MAPSIZE-1);
			// ������㥬 ��砩��� �祩��
			int row = randomMapNumber(rng);
			int column = randomMapNumber(rng);
			// �᫨ �� �⠪㥬 ����� ࠧ � �����頥�
			if (!this->isAlreadyAttacked(row, column))
				return {row, column};
			// ���� - ��ॣ�����㥬 �祩��
			return this->generateRandomCell();
		}
		
		// ��⮤, �஢���� �⠪���� �� ��� �祪� {row, column}
		bool isAlreadyAttacked(int row, int column) const {
			// ��ॡ�ࠥ� ����� � ᬮ�ਬ, �⠪����� �� ��� �� �� �⮣�
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

	// ������ ��ꥪ�� ��� � ��ப�
	Bot bot;
	Player player;
	//  � ��᪮��筮� 横��
	while (true) {
		///// �᫨ �᪮�����஢��� ��ப� ���� - � ���� ��� ⮦� �㤥� ��������
		// bot.printMaps();

		// �뢮��� ����� ��ப�
		player.printMaps();

		{ // ����訢��� �祪� � ��ப�
			auto [row, column] = player.getAttackCell();
			// �஢���� �⠪� �� ��� � �⮡ࠦ��� १���� �� ���� �⠪ ��ப�
			player.applyAttackAtAttackMap(row, column, bot.applyAttackAtShipMap(row, column));
		}
		{
			 // ����訢��� �祪� � ��� (���������� ��砩��)
			auto [row, column] = bot.getAttackCell();
			// �뢮��� �� �࠭, �㤠 �⠪���� ���
			std::cout << "\n��� �⠪�� � " << char(column+97) << row+1 << "\n\n";
			// �஢���� �⠪� �� ��ப� � �⮡ࠦ��� १���� �� ���� �⠪ ���
			bot.applyAttackAtAttackMap(row, column, player.applyAttackAtShipMap(row, column));
		}

	
		// �஢��塞, ����� � ��� 㦥 ��� ��ࠡ���
		if (bot.isAllShipsDefeated()) {
			// �᫨ � ��� ���稫��� ��ࠡ�� - �뢮��� �������, �� �� ��������!
			cout <<  " __   __  _______  __   __          _     _  ___   __    _  __   __   __ " << "\n"
				 <<  "|  | |  ||       ||  | |  |        | | _ | ||   | |  |  | ||  | |  | |  | " << "\n"
				 <<  "|  |_|  ||   _   ||  | |  |        | || || ||   | |   |_| ||  | |  | |  | " << "\n"
				 <<  "|       ||  | |  ||  |_|  |        |       ||   | |       ||  | |  | |  | " << "\n"
				 <<  "|_     _||  |_|  ||       |        |       ||   | |  _    ||__| |__| |__| " << "\n"
  				 <<  "  |   |  |       ||       |        |   _   ||   | | | |   | __   __   __  " << "\n"
				 <<  "  |___|  |_______||_______|        |__| |__||___| |_|  |__||__| |__| |__|" << "\n";
			break;// � �����蠥� 横�
		}
		// �஢��塞, ����� � ��ப� 㦥 ��� ��ࠡ���
		if (player.isAllShipsDefeated()) {
			// �᫨ � ��ப� ���稫��� ��ࠡ�� - �뢮��� �������, �� �� �ந�ࠫ�!
			cout << " __   __  _______  __   __          ___      _______  _______  _______ "<< "\n"
				 << "|  | |  ||       ||  | |  |        |   |    |       ||       ||       |"<< "\n"
				 << "|  |_|  ||   _   ||  | |  |        |   |    |   _   ||  _____||    ___|"<< "\n"
				 << "|       ||  | |  ||  |_|  |        |   |    |  | |  || |_____ |   |___ "<< "\n"
				 << "|_     _||  |_|  ||       |        |   |___ |  |_|  ||_____  ||    ___|"<< "\n"
  				 << "  |   |  |       ||       |        |       ||       | _____| ||   |___ "<< "\n"
  				 << "  |___|  |_______||_______|        |_______||_______||_______||_______|"<< "\n";
			break; // � �����蠥� 横�
		}
	}

	return 0;
}
