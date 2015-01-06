#include "BuildingCard.h"

/**
*	Create default card
**/
BuildingCard::BuildingCard()
{
	this->setName("NOT SET");
	this->setCost(-1);
	this->setColor(Colors::NOT_SET);
	this->setDescription("NOT SET");
}

/**
*	Create initialized card
**/
BuildingCard::BuildingCard(std::string name, int cost, Colors color, std::string description) {
	this->setName(name);
	this->setCost(cost);
	this->setColor(color);
	this->setDescription("NOT SET");
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

std::string BuildingCard::toString() {
	std::string retVal = "Name: ";
	retVal.append(this->getName());
	retVal.append(", cost: ");
	retVal.append(std::to_string(this->getCost()));
	retVal.append(", color: ");
	retVal.append(this->getColorString());

	return retVal;
	

}

/**
*	Destructor
**/
BuildingCard::~BuildingCard()
{
}
