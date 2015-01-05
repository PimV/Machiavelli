#pragma once
#include <iostream>
#include "Color.h"

class BuildingCard
{
public:
	BuildingCard();
	BuildingCard(std::string name, int cost, Color color, std::string description);

	void setName(std::string name);
	std::string getName();

	void setCost(int cost);
	int getCost();

	void setColor(Color color);
	Color getColor();
	std::string getColorString();

	void setDescription(std::string description);
	std::string getDescription();

	~BuildingCard();
private:
	std::string name;
	int cost;
	Color color;
	std::string description;
};

