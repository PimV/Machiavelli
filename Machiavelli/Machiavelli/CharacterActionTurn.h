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

	void doSpecial();
	bool didSpecial();

	void finishTurn();

	bool isOver();

	void resetTurn();

	virtual ~CharacterActionTurn();
private:
	bool goldTaken;
	bool pulledCard;
	bool singleCardTaken;
	bool executedSpecial;
	bool finished;
};

