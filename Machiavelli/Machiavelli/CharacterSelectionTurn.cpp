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
	std::string actions = "Mogelijke acties voor deze beurt: \r\n";
	if (!this->hasCoveredCard()) {
		actions.append("\t - dek <index> \r\n");
	}

	if (!this->hasPickedCard()) {
		actions.append("\t - pak <index> \r\n");
	}

	actions.append("\t - bekijk_alle_karakterkaarten\r\n");
	return actions;
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
