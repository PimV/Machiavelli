#pragma once
#include <iostream>
#include <memory>
#include <vector>
#include <map>

#include "Player.h"
class Game;
class InputHandler
{
public:
	InputHandler();

	void setGame(std::shared_ptr<Game> game);
	std::shared_ptr<Game> getGame();

	bool isGlobalCommand(std::string command);
	bool isGameCommand(std::string command);
	bool isTurnIndependentCommand(std::string command);

	bool handleInput(std::string input, std::shared_ptr<Player> player);
	void handleGameCommand(std::vector<std::string> params, std::shared_ptr<Player> player);
	void handleGlobalCommand(std::vector<std::string> params, std::shared_ptr<Player> player);

	//Input actions
	/* Turns */
	void finishTurn(std::vector<std::string> params, std::shared_ptr<Player> player);

	/* Character Selection */
	void coverCard(std::vector<std::string> params, std::shared_ptr<Player> player);
	void pickCard(std::vector<std::string> params, std::shared_ptr<Player> player);

	/* Character Action */
	void selectBuildCard(std::vector<std::string> params, std::shared_ptr<Player> player);
	void build(std::vector<std::string> params, std::shared_ptr<Player> player);
	void checkGold(std::vector<std::string> params, std::shared_ptr<Player> player);

	/* Moordenaar */
	void murder(std::vector<std::string> params, std::shared_ptr<Player> player);
	void murderOptions(std::vector<std::string> params, std::shared_ptr<Player> player);

	/* Dief */
	void pickpocket(std::vector<std::string> params, std::shared_ptr<Player> player);
	void pickpocketOptions(std::vector<std::string> params, std::shared_ptr<Player> player);

	/* Magiër */
	void swapHand(std::vector<std::string> params, std::shared_ptr<Player> player);
	void swapCards(std::vector<std::string> params, std::shared_ptr<Player> player);

	/* Condottiere */
	void remove(std::vector<std::string> params, std::shared_ptr<Player> player);
	void removeOptions(std::vector<std::string> params, std::shared_ptr<Player> player);

	/* Buildings */
	void labSpecial(std::vector<std::string> params, std::shared_ptr<Player> player);

	//Random
	std::vector<std::string> splitOnSpace(std::string input);
	std::vector<std::string> splitOnComma(std::string input);
	void updatePreviousCommands(std::string input, std::shared_ptr<Player>);

	int stringToInt(std::string input);

	virtual ~InputHandler();
private:
	std::shared_ptr<std::map<std::shared_ptr<Player>, std::shared_ptr<std::vector<std::string>>>> previousCommands;
	std::shared_ptr<Game> game;
	std::map<std::string, std::string> gameCmds;
	std::map<std::string, std::string> tiCmds;
	std::map<std::string, std::string> globalCmds;


	std::vector<std::string> gameCommands;
	std::vector<std::string> globalCommands;
	std::vector<std::string> turnIndependentCommands;
};

