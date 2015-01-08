#include "CharacterSelectionTurn.h"


CharacterSelectionTurn::CharacterSelectionTurn()
{
	this->setType(Turns::CHAR_SELECT);
	coveredCard = false;
	pickedCard = false;
}

void CharacterSelectionTurn::coverCard() {
	this->coveredCard = true;
}

void CharacterSelectionTurn::pickCard() {
	this->pickedCard = true;
}

bool CharacterSelectionTurn::hasCoveredCard() {
	return this->coveredCard;
}

bool CharacterSelectionTurn::hasPickedCard() {
	return this->pickedCard;
}

bool CharacterSelectionTurn::isOver() {
	if (this->coveredCard && this->pickedCard) {
		return true;
	}
	return false;
}

std::string CharacterSelectionTurn::printPossibleActions() {
	return "std::string";
}

void CharacterSelectionTurn::resetTurn() {
	this->coveredCard = false;
	this->pickedCard = false;
}

std::string CharacterSelectionTurn::toString() {
	return this->getPlayer()->getName() + " is aan de beurt.";
}


CharacterSelectionTurn::~CharacterSelectionTurn()
{
}
