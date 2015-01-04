#pragma once
#include <iostream>
#include "Color.h"

class BuildingCard
{
public:
	BuildingCard();
	BuildingCard(std::string name, int cost, Color color);

	void setName(std::string name);
	std::string getName();

	void setCost(int cost);
	int getCost();

	void setColor(Color color);
	Color getColor();
	std::string getColorString();

	~BuildingCard();
private:
	std::string name;
	int cost;
	Color color;
};

