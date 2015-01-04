#include "CharacterCard.h"

/**
*	Create default card
**/
CharacterCard::CharacterCard()
{
	this->setName("NOT SET");
	this->setId(-1);
}

/**
*	Create initialized card
**/
CharacterCard::CharacterCard(std::string name, int id) {
	this->setName(name);
	this->setId(id);
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

/**
*	Destructor
**/
CharacterCard::~CharacterCard()
{
}
