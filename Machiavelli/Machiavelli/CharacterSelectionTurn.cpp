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

void CharacterSelectionTurn::resetTurn() {
	this->coveredCard = false;
	this->pickedCard = false;
	this->getPlayer()->getClient()->write("U bent aan de beurt en moet nu één kaart bedekken (dek <kaart_index>) en één kaart pakken (pak <kaart_index>).\r\n");
}


CharacterSelectionTurn::~CharacterSelectionTurn()
{
}
