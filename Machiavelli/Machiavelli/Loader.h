#pragma once
#include "Colors.h"
#include "Characters.h"
#include "Buildings.h"

#include "BuildingCard.h"
#include "CharacterCard.h"
#include "Deck.h"

class Loader
{
public:
	Loader();

	std::shared_ptr<Deck<std::shared_ptr<BuildingCard>>> loadBuildingCards(std::shared_ptr<Deck<std::shared_ptr<BuildingCard>>> deck);
	std::shared_ptr<Deck<std::shared_ptr<CharacterCard>>> loadCharacterCards(std::shared_ptr<Deck<std::shared_ptr<CharacterCard>>> deck);

	bool isSpecialBuildingCard(std::shared_ptr<BuildingCard> card);

	virtual ~Loader();
};

