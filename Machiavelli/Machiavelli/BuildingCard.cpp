#include "BuildingCard.h"

/**
*	Create default card
**/
BuildingCard::BuildingCard()
{
	this->setName("NOT SET");
	this->setCost(-1);
	this->setColor(Color::NOT_SET);
}

/**
*	Create initialized card
**/
BuildingCard::BuildingCard(std::string name, int cost, Color color) {
	this->setName(name);
	this->setCost(cost);
	this->setColor(color);
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
void BuildingCard::setColor(Color color) {
	this->color = color;
}

/**
*	Get card color as enum value
**/
Color BuildingCard::getColor() {
	return this->color;
}

/**
*	Get card color as string
**/
std::string BuildingCard::getColorString() {
	return color_strings[static_cast<int>(this->color)];
}

/**
*	Destructor
**/
BuildingCard::~BuildingCard()
{
}
