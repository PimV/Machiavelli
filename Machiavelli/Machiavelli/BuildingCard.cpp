#include "BuildingCard.h"
//#include <Windows.h>
/**
*	Create default card
**/
BuildingCard::BuildingCard()
{
	this->setName("NOT SET");
	this->setCost(-1);
	this->setColor(Colors::NOT_SET);
	this->setDescription("NOT SET");
	this->setBuilding(Buildings::NOT_SET);
	this->specialCard = false;
	this->specialExecuted = false;
}

/**
*	Create initialized card
**/
BuildingCard::BuildingCard(std::string name, Buildings building, int cost, Colors color, std::string description) {
	this->setName(name);
	this->setCost(cost);
	this->setColor(color);
	this->setBuilding(building);
	this->setDescription("NOT SET");
	this->specialCard = false;
	this->specialExecuted = false;
}

/**
*	Set card name
**/
void BuildingCard::setName(std::string name) {
	this->name = name;
}

/**
*	Get card name
**/
std::string BuildingCard::getName() {
	return this->name;
}

void BuildingCard::setBuilding(Buildings building) {
	this->building = building;
}

Buildings BuildingCard::getBuilding() {
	return this->building;
}

/**
*	Set card cost
**/
void BuildingCard::setCost(int cost) {
	this->cost = cost;
}

/**
*	Get card cost
**/
int BuildingCard::getCost() {
	return this->cost;
}

int BuildingCard::getDestroyCost() {
	if (this->cost == 1) {
		return 0;
	}
	else {
		return this->cost - 1;
	}
}

/**
*	Set card color
**/
void BuildingCard::setColor(Colors color) {
	this->color = color;
}

/**
*	Get card color as enum value
**/
Colors BuildingCard::getColor() {
	return this->color;
}

/**
*	Get card color as string
**/
std::string BuildingCard::getColorString() {
	return color_strings[static_cast<int>(this->color)];
}

/**
*   Set card description
**/
void BuildingCard::setDescription(std::string description) {
	this->description = description;
}

/**
*   Get card description
**/
std::string BuildingCard::getDescription() {
	return this->description;
}

void BuildingCard::giveSpecial() {
	this->specialCard = true;
}

bool BuildingCard::hasSpecial() {
	return this->specialCard;
}

bool BuildingCard::didSpecial() {
	return this->specialExecuted;
}


void BuildingCard::doSpecial() {
	this->specialExecuted = true;
}

std::string BuildingCard::toString() {
	std::string retVal = "Name: ";
	retVal.append(this->getName());
	retVal.append(", cost: ");
	retVal.append(std::to_string(this->getCost()));
	retVal.append(", color: ");
	retVal.append(this->getColorString());

	return retVal;
}

void BuildingCard::reset() {
	this->specialExecuted = false;
}

/**
*	Destructor
**/
BuildingCard::~BuildingCard()
{
	//OutputDebugStringW(L"Verwijder bouwkaart\r\n"); // include <Windows.h> to test
}
