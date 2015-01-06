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

	bool isGameCommand(std::string command);

	bool handleInput(std::string input, std::shared_ptr<Player> player);
	void handleGameCommand(std::vector<std::string> params, std::shared_ptr<Player> player);
	void handleGlobalCommand(std::vector<std::string> params, std::shared_ptr<Player> player);
	std::vector<std::string> splitOnSpace(std::string input);
	void updatePreviousCommands(std::string input, std::shared_ptr<Player>);

	virtual ~InputHandler();
private:
	std::shared_ptr<std::map<std::shared_ptr<Player>, std::shared_ptr<std::vector<std::string>>>> previousCommands;
	std::shared_ptr<Game> game;
	std::vector<std::string> gameCommands;
	std::vector<std::string> globalCommands;
};	

