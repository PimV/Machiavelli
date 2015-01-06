#pragma once
#include <iostream>
#include <string>
#include "Colors.h"

class BuildingCard
{
public:
	BuildingCard();
	BuildingCard(std::string name, int cost, Colors color, std::string description);

	void setName(std::string name);
	std::string getName();

	void setCost(int cost);
	int getCost();

	void setColor(Colors color);
	Colors getColor();
	std::string getColorString();

	void setDescription(std::string description);
	std::string getDescription();

	std::string toString();

	~BuildingCard();
private:
	std::string name;
	int cost;
	Colors color;
	std::string description;
};

