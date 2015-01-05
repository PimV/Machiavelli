#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <memory>

#include "Loader.h"


Loader::Loader()
{
}

std::shared_ptr<Deck<std::shared_ptr<BuildingCard>>> Loader::loadBuildingCards(std::shared_ptr<Deck<std::shared_ptr<BuildingCard>>> deck) {
	std::ifstream  file("Bouwkaarten.csv");
	
	std::string line;

	while (std::getline(file, line))
	{
		std::stringstream  lineStream(line);
		std::string        cell;

		std::string name = "NOT SET";
		int cost = -1;
		std::string colorString;
		Color color = Color::NOT_SET;
		std::string description = "NOT SET";

		int i = 0;

		while (std::getline(lineStream, cell, ';'))
		{
			if (i == 0) {
				name = cell;
			}
			else if (i == 1) {
				cost = std::stoi(cell);
			}
			else if (i == 2) {
				colorString = cell;
				if (colorString == "blauw") {
					color = Color::Blauw;
				}
				else if (colorString == "lila") {
					color = Color::Lila;
				}
				else if (colorString == "groen") {
					color = Color::Groen;
				}
				else if (colorString == "rood") {
					color = Color::Rood;
				}
				else if (colorString == "geel") {
					color = Color::Geel;
				}

			}
			else if (i == 3) {
				description = cell;
			}

			i++;
		}
		std::shared_ptr<BuildingCard> card = std::make_shared<BuildingCard>(name, cost, color, description);
		deck->push(card);	
	}
	return deck;
}

std::shared_ptr<Deck<std::shared_ptr<CharacterCard>>> Loader::loadCharacterCards(std::shared_ptr<Deck<std::shared_ptr<CharacterCard>>> deck) {
	std::ifstream  file("karakterkaarten.csv");

	std::string line;

	while (std::getline(file, line))
	{
		std::stringstream  lineStream(line);
		std::string        cell;

		std::string name = "NOT SET";
		int id = -1;

		int i = 0;

		while (std::getline(lineStream, cell, ';'))
		{
			if (i == 0) {
				id = stoi(cell);
			}
			else if (i == 1) {
				name = cell;
			}

			i++;
		}
		std::shared_ptr<CharacterCard> card = std::make_shared<CharacterCard>(name, id);
		deck->push(card);
	}
	return deck;
}


Loader::~Loader()
{
}
