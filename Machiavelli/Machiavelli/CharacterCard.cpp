#include "CharacterCard.h"

/**
*	Create default card
**/
CharacterCard::CharacterCard()
{
	this->setName("NOT SET");
	this->setId(-1);
	this->setCharacter(Characters::NOT_SET);
}

/**
*	Create initialized card
**/
CharacterCard::CharacterCard(std::string name, int id, Characters character) {
	this->setName(name);
	this->setId(id);
	this->setCharacter(character);
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

std::string CharacterCard::toString() {
	std::string retVal = "Name: ";
	retVal.append(this->getCharacterString());
	retVal.append(", id: ");
	retVal.append(std::to_string(this->getId()));

	return retVal;
}

/**
*	Destructor
**/
CharacterCard::~CharacterCard()
{
}
