#pragma once
#include <memory>
#include "Loader.h"
#include "Deck.h"
#include "BuildingCard.h"
#include "CharacterCard.h"
#include "Player.h"
#include "BaseTurn.h"
#include "CharacterSelectionTurn.h"
#include "CharacterActionTurn.h"
#include "InputHandler.h"

class Game 
{
public:
	Game();

	//Start game
	void startGame();
	void startRound();
	bool isStarted();

	//Turn handling
	bool checkTurnSwitch();
	void switchTurn();
	void switchTurnTypes();
	void prepareTurn();
	bool correctPlayerTurn(std::shared_ptr<Player> player);
	std::shared_ptr<BaseTurn> getTurn();

	//Action handling
	/* Character Card Selection */
	bool validateCharacterCardInput(int index);
	void pickCharacterCard(std::shared_ptr<Player> player, int index);
	void coverCharacterCard(std::shared_ptr<Player> player, int index);
	
	/* Character Card Execution */
	void callCharacterCard();
	void takeGold(std::shared_ptr<Player> player);
	void takeCards(std::shared_ptr<Player> player);
	void selectBuildingCard(std::shared_ptr<Player> player, int index);

	//Player handling
	std::shared_ptr<Player> addPlayer(std::shared_ptr<Socket> client);
	void removePlayer(std::shared_ptr<Socket> client);
	std::shared_ptr<Player> getPlayerByClient(std::shared_ptr<Socket> client);
	int getPlayerCount();

	void showCharacterDeckOptions(std::shared_ptr<Player> player);

	//Get card decks
	std::shared_ptr<Deck<std::shared_ptr<CharacterCard>>> getCharacterDeck();
	std::shared_ptr<Deck<std::shared_ptr<BuildingCard>>> getBuildingDeck();

	//Input handling

	virtual ~Game();

private:
	std::unique_ptr<Loader> loader;

	std::shared_ptr<Deck<std::shared_ptr<CharacterCard>>> characterOrderDeck;
	std::shared_ptr<Deck<std::shared_ptr<CharacterCard>>> characterDeck;
	std::shared_ptr<Deck<std::shared_ptr<BuildingCard>>> buildingDeck;

	std::shared_ptr<Player> player1;
	std::shared_ptr<Player> player2;

	std::shared_ptr<BaseTurn> turn;

	bool started;

	int turns;
	int rounds;
	
	int currentCharacterCardIndex;


};

