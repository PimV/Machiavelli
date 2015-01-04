#pragma once
#include <iostream>

class CharacterCard
{
public:
	CharacterCard();
	CharacterCard(std::string name, int id);

	void setId(int id);
	int getId();

	void setName(std::string name);
	std::string getName();

	virtual ~CharacterCard();
private:
	int id;
	std::string name;

};

