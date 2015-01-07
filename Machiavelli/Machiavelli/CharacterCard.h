#pragma once
#include <iostream>
#include <string>
#include "Characters.h"

class CharacterCard
{
public:
	CharacterCard();
	CharacterCard(std::string name, int id, Characters character);

	void setId(int id);
	int getId();

	void setName(std::string name);
	std::string getName();

	void setCharacter(Characters character);
	Characters getCharacter();
	std::string getCharacterString();

	std::string possibleActions();

	std::string toString();


	virtual ~CharacterCard();
private:
	int id;
	std::string name;
	Characters character;

};

