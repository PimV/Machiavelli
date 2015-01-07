#pragma once
#include "BaseTurn.h"

class CharacterActionTurn :
	public BaseTurn
{
public:
	CharacterActionTurn();

	void takeGold();
	bool tookGold();

	void takeCardsFromDeck();
	bool tookCards();

	void selectSingleCardFromDeck();
	bool tookSingleCard();

	int getMaxBuildings();
	void setMaxBuildings(int maxBuildings);
	void buildBuilding();
	bool buildingsBuilt();

	void doSpecial();
	bool didSpecial();

	void finishTurn();

	bool isOver();

	void resetTurn();

	std::string toString();

	virtual ~CharacterActionTurn();
private:
	bool goldTaken;
	bool pulledCard;
	bool singleCardTaken;
	int maxBuildings;
	int builtBuildings;
	bool executedSpecial;
	bool finished;
};

