#pragma once
#include <iostream>
#include <memory>
#include <vector>
#include "Socket.h"
#include "CharacterCard.h"
#include "BuildingCard.h"

class Player
{
public:
	Player();
	Player(std::string name, int age);

	void setClient(std::shared_ptr<Socket> client);
	std::shared_ptr<Socket> getClient();

	void setName(std::string name);
	std::string getName();

	void setAge(int age);
	int getAge();

	void setKing(bool isKing);
	bool playerIsKing();


	/* Character Cards */
	bool takeCharacterCard(std::shared_ptr<CharacterCard> card);
	bool hasCharacterChard(std::shared_ptr<CharacterCard> card);
	bool hasCharacterCardByCharacter(Characters character);

	/* Gold */
	void changeGoldBy(int amount);
	int getGold();

	/* Active Character Card after call */
	void setActiveCharacterCard(std::shared_ptr<CharacterCard> card);
	std::shared_ptr<CharacterCard> getActiveCharacterCard();

	/* Building Cards */
	void addBuildingCard(std::shared_ptr<BuildingCard> card);
	void removeBuildingCard(std::shared_ptr<BuildingCard> card);
	std::vector<std::shared_ptr<BuildingCard>> getBuildingCards();
	void setBuildingCards(std::vector<std::shared_ptr<BuildingCard>> cards);
	void printBuildingCards();

	/* Choosable Building Cards*/
	void addChoosableBuildingCards(std::shared_ptr<BuildingCard> card);
	std::shared_ptr<BuildingCard> getChoosableBuildingCardByIndex(int index);
	void emptyChoosableBuildingCards();
	void printChoosableBuildingCards();

	/* Buildings */
	bool constructBuildingCard(int index);

	/* Reset */
	void resetForNextRound();


	virtual ~Player();
private:
	std::string name;
	int age;
	int gold;

	bool isKing;
	bool turn;

	std::shared_ptr<CharacterCard> character1;
	std::shared_ptr<CharacterCard> character2;
	std::shared_ptr<CharacterCard> activeCharacter;

	std::vector<std::shared_ptr<BuildingCard>> buildings;
	std::vector<std::shared_ptr<BuildingCard>> buildingCards;
	std::vector<std::shared_ptr<BuildingCard>> choosableBuildingCards;

	std::shared_ptr<Socket> client;

};

