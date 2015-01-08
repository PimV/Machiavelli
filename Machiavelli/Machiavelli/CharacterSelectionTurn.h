#pragma once
#include "BaseTurn.h"

class CharacterSelectionTurn :
	public BaseTurn
{
public:
	CharacterSelectionTurn();

	void coverCard();
	void pickCard();

	bool hasCoveredCard();
	bool hasPickedCard();

	bool isOver();

	void resetTurn();

	std::string printPossibleActions();

	std::string toString();

	virtual ~CharacterSelectionTurn();
private:
	bool coveredCard;
	bool pickedCard;
};

