#include "CharacterActionTurn.h"


CharacterActionTurn::CharacterActionTurn()
{
	this->setType(Turns::CHAR_ACTION);
	goldTaken = false;
	pulledCard = false;
	singleCardTaken = false;
	executedSpecial = false;
	finished = false;
}

bool CharacterActionTurn::isOver() {
	if (goldTaken) {
		return true;
	}

	if (singleCardTaken) {
		return true;
	}

	return false;
}

void CharacterActionTurn::doSpecial() {
	this->executedSpecial = true;
}

bool CharacterActionTurn::didSpecial() {
	return this->executedSpecial;
}

void CharacterActionTurn::takeGold() {
	this->goldTaken = true;
}

bool CharacterActionTurn::tookGold() {
	return this->goldTaken;
}

void CharacterActionTurn::takeCardsFromDeck() {
	this->pulledCard = true;
}

bool CharacterActionTurn::tookCards() {
	return this->pulledCard;
}

void CharacterActionTurn::selectSingleCardFromDeck() {
	singleCardTaken = true;
}

bool CharacterActionTurn::tookSingleCard() {
	return 	singleCardTaken;
}

void CharacterActionTurn::resetTurn() {
	goldTaken = false;
	pulledCard = false;
	singleCardTaken = false;
	executedSpecial = false;
	finished = false;
}

std::string CharacterActionTurn::toString() {
	return this->getPlayer()->getName() + " is aan de beurt met de " + this->getPlayer()->getActiveCharacterCard()->getCharacterString() + ".";
}


CharacterActionTurn::~CharacterActionTurn()
{
}
