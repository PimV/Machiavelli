#pragma once
#include <iostream>
#include <string>
#include "Colors.h"
#include "Buildings.h"

class BuildingCard
{
public:
	BuildingCard();
	BuildingCard(std::string name, Buildings building, int cost, Colors color, std::string description);

	void setName(std::string name);
	std::string getName();

	void setBuilding(Buildings building);
	Buildings getBuilding();

	void setCost(int cost);
	int getCost();
	int getDestroyCost();

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
	Buildings building;
};

