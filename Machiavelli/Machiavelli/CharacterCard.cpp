#include "CharacterCard.h"

/**
*	Create default card
**/
CharacterCard::CharacterCard()
{
	this->setName("NOT SET");
	this->setId(-1);
	this->setCharacter(Characters::NOT_SET);
	this->setMurdered(false);
	this->setPickpocketed(false);
}

/**
*	Create initialized card
**/
CharacterCard::CharacterCard(std::string name, int id, Characters character) {
	this->setName(name);
	this->setId(id);
	this->setCharacter(character);
	this->setMurdered(false);
	this->setPickpocketed(false);
}

/**
*	Set card name
**/
void CharacterCard::setName(std::string name) {
	this->name = name;
}

/**
*	Get card name
**/
std::string CharacterCard::getName() {
	return this->name;
}

/**
*	Set card character
**/
void CharacterCard::setCharacter(Characters character) {
	this->character = character;
}

/**
*	Get card character
**/
Characters CharacterCard::getCharacter() {
	return this->character;
}

/**
*	Get card character string
**/
std::string CharacterCard::getCharacterString() {
	return character_strings[(int)this->character];
}

/**
*	Set card id
**/
void CharacterCard::setId(int id) {
	this->id = id;
}

/**
*	Get card id
**/
int CharacterCard::getId() {
	return this->id;
}

void CharacterCard::setPickpocketed(bool pickpocketed) {
	this->pickpocketed = pickpocketed;
}

bool CharacterCard::isPickpocketed() {
	return this->pickpocketed;
}

void CharacterCard::setMurdered(bool murdered) {
	this->murdered = murdered;
}

bool CharacterCard::isMurdered() {
	return this->murdered;
}

std::string CharacterCard::possibleActions() {
	std::string actions = "Mogelijke acties voor de " + this->getCharacterString() + "\r\n";
	actions.append("\t - beurt_over \r\n");
	actions.append("\t - pak_goud \r\n");
	actions.append("\t - pak_bouwkaarten \r\n");
	actions.append("\t - leg_bouwkaart <index> \r\n");

	switch (this->getCharacter()) {
	case Characters::Moordenaar:
		actions.append("\t - vermoord <karakter_index> \r\n");
		actions.append("\t - vermoord_opties \r\n");
		break;
	case Characters::Dief:
		actions.append("\t - besteel_opties \r\n");
		actions.append("\t - besteel <karakter_index> \r\n");
		break;
	case Characters::Magier:
		actions.append("\t - ruil_hand \r\n");
		actions.append("\t - ruil_kaarten <kaart1, kaart2, ...>\r\n");
		break;
	case Characters::Koning:

		break;
	case Characters::Prediker:

		break;
	case Characters::Koopman:
		break;
	case Characters::Bouwmeester:

		break;
	case Characters::Condottiere:
		actions.append("\t - verwijder_gebouw <gebouw_index>\r\n");
		break;
	default:
		break;
	}

	actions.append("\t - help \r\n");
	return actions;
}

std::string CharacterCard::toString() {
	//std::string retVal = "";
	//retVal.append(this->getCharacterString());
	//retVal.append(", id: ");
	//retVal.append(std::to_string(this->getId()));
	return this->getCharacterString();
	//return retVal;
}

/**
*	Destructor
**/
CharacterCard::~CharacterCard()
{
}
