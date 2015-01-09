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
	void endGame();
	void startRound();
	std::shared_ptr<CharacterCard> getCurrentCalledCharacter();
	bool isStarted();

	//Turn handling
	bool checkTurnSwitch();
	void switchTurn();
	void switchTurnTypes();
	void prepareTurn();
	void applyCardEffects(std::shared_ptr<Player> player);
	bool correctPlayerTurn(std::shared_ptr<Player> player);
	bool correctCharacterTurn(std::shared_ptr<Player> player, Characters character);
	bool correctBuildingTurn(std::shared_ptr<Player> player, Buildings building);
	std::shared_ptr<BaseTurn> getTurn();
	void printPossibleActions(std::shared_ptr<Player> player);

	//Action handling
	/* Character Card Selection */
	bool validateCharacterCardInput(int index);
	void pickCharacterCard(std::shared_ptr<Player> player, int index);
	void coverCharacterCard(std::shared_ptr<Player> player, int index);
	
	/* Character Card Execution */
	void callCharacterCard();
	void takeGold(std::shared_ptr<Player> player);
	void takeCards(std::shared_ptr<Player> player);
	void constructBuilding(std::shared_ptr<Player> player, int index);
	void removeBuilding(std::shared_ptr<Player> player, int index);
	void printRemoveOptions(std::shared_ptr<Player> player);
	void printOpponentBuildings(std::shared_ptr<Player> player);
	void selectBuildingCard(std::shared_ptr<Player> player, int index);
	void printCharacterOrderCard(std::shared_ptr<Player> player);

	void murderCharacter(std::shared_ptr<Player> player, int index);
	void pickpocketCharacter(std::shared_ptr<Player> player, int index);

	void swapHands(std::shared_ptr<Player> player);
	void swapCards(std::shared_ptr<Player> player, std::vector<int> indices);

	void workshopSpecial(std::shared_ptr<Player> player);
	void labSpecial(std::shared_ptr<Player> player, int index);


	//Player handling
	std::shared_ptr<Player> addPlayer(std::shared_ptr<Socket> client);
	void removePlayer(std::shared_ptr<Socket> client);
	std::shared_ptr<Player> getPlayerByClient(std::shared_ptr<Socket> client);
	int getPlayerCount();

	void showCharacterDeckOptions(std::shared_ptr<Player> player);

	//Get card decks
	std::shared_ptr<Deck<std::shared_ptr<CharacterCard>>> getCharacterDeck();
	std::shared_ptr<Deck<std::shared_ptr<BuildingCard>>> getBuildingDeck();

	void printOnTurn(std::shared_ptr<Player> player);

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
	bool lastRound;
	bool gameOver;

	int turns;
	int rounds;
	
	int currentCharacterCardIndex;


};

