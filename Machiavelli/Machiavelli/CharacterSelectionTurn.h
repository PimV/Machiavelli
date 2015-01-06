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

	virtual ~CharacterSelectionTurn();
private:
	bool coveredCard;
	bool pickedCard;
};

