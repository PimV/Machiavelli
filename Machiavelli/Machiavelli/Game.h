#pragma once
#include <memory>
#include "Loader.h"
#include "Deck.h"
#include "BuildingCard.h"
#include "CharacterCard.h"
#include "Player.h"

class Game
{
public:
	Game();

	std::shared_ptr<Deck<std::shared_ptr<CharacterCard>>> getCharacterDeck();
	std::shared_ptr<Deck<std::shared_ptr<BuildingCard>>> getBuildingDeck();

	std::shared_ptr<Player> addPlayer(std::shared_ptr<Socket> client);
	void removePlayer(std::shared_ptr<Socket> client);
	int getPlayerCount();

	virtual ~Game();

private:
	std::unique_ptr<Loader> loader;
	std::shared_ptr<Deck<std::shared_ptr<CharacterCard>>> characterDeck;
	std::shared_ptr<Deck<std::shared_ptr<BuildingCard>>> buildingDeck;
	
	std::shared_ptr<Player> player1;
	std::shared_ptr<Player> player2;
	
};

