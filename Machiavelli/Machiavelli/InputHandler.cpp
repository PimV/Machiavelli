#include "InputHandler.h"
#include <string>
#include <sstream>
#include <algorithm>
#include "Server.h"
#include "Game.h"

InputHandler::InputHandler()
{
	previousCommands = std::make_shared<std::map<std::shared_ptr<Player>, std::shared_ptr<std::vector<std::string>>>>();
	gameCommands.push_back("pak_goud");
	gameCommands.push_back("pak_kaarten");
	gameCommands.push_back("selecteer_bouwkaart");
	gameCommands.push_back("beurt_over");
	gameCommands.push_back("bekijk_bouwkaarten");
	gameCommands.push_back("pak");
	gameCommands.push_back("dek");
}

bool InputHandler::isGameCommand(std::string command) {
	if (std::find(gameCommands.begin(), gameCommands.end(), command) != gameCommands.end())
	{
		return true;
	}
	return false;
}

bool InputHandler::handleInput(std::string input, std::shared_ptr<Player> player) {
	if (input.length() > 0) {

		std::vector<std::string> params = this->splitOnSpace(input);

		//Separate function for game commands
		if (isGameCommand(params[0])) {
			this->handleGameCommand(params, player);
			return true;
		}
		else {
			this->handleGlobalCommand(params, player);
		}

		//Catch input and execute appropriate action


		this->updatePreviousCommands(input, player);
	}
	return true;
}

void InputHandler::handleGameCommand(std::vector<std::string> params, std::shared_ptr<Player> player) {
	if (!this->game->isStarted()) {
		player->getClient()->write("Het spel is nog niet begonnen!\r\n");
		return;
	}
	if (params[0] == "beurt_over") {
		if (this->game->checkTurnSwitch()) {
			this->game->switchTurn();
		}
		else {
			player->getClient()->write("U moet minimaal 2 goud óf 2 bouwkaarten pakken per beurt.");
		}
	}
	else if (params[0] == "pak_goud") {
		this->game->takeGold(player);
	}
	else if (params[0] == "pak_kaarten") {
		this->game->takeCards(player);
	}
	else if (params[0] == "bekijk_bouwkaarten") {
		player->printBuildingCards();
	}
	else if (params[0] == "selecteer_bouwkaart") {
		if (params.size() > 1) {
			int index = stoi(params[1]);
			index = index - 1; // correct for + 1 visible to user
			this->game->selectBuildingCard(player, index);
		}
		else {
			player->getClient()->write("Incorrect gebruik. Voorbeeld: selecteer_bouwkaart 1\r\n");
		}
	}
	else if (params[0] == "pak") {
		if (params.size() > 1) {
			int index = stoi(params[1]);
			index = index - 1; // correct for + 1 visible to user
			this->game->pickCharacterCard(player, index);
		}
		else {
			player->getClient()->write("Incorrect gebruik. Voorbeeld: pak 1\r\n");
		}
	}
	else if (params[0] == "dek") {
		if (params.size() > 1) {
			int index = stoi(params[1]);
			index = index - 1; // correct for + 1 visible to user
			this->game->coverCharacterCard(player, index);
		}
		else {
			player->getClient()->write("Incorrect gebruik. Voorbeeld: dek 1\r\n");
		}
	}
}

void InputHandler::handleGlobalCommand(std::vector<std::string> params, std::shared_ptr<Player> player) {
	if (params[0] == "broadcast") {
		if (params.size() > 1) {
			Server::Instance().broadcast(params[1]);
		}
	}
	else if (params[0] == "historie") {
		player->getClient()->write("Previous commands: \r\n");
		for (std::vector<std::string>::size_type i = 0; i != previousCommands->at(player)->size(); i++) {
			player->getClient()->write(previousCommands->at(player)->at(i) + "\r\n");
		}

	}
	else if (params[0] == "naam") {
		if (params.size() > 1) {
			player->setName(params[1]);
			player->getClient()->write("Naam veranderd in: [" + params[1] + "]\r\n");
		}
	}
	else if (params[0] == "disconnect") {

	}
	else {
		player->getClient()->write("Ongeldig commando ingevoerd.\r\n");
	}
}

void InputHandler::updatePreviousCommands(std::string input, std::shared_ptr<Player> player) {
	if (!this->previousCommands->count(player)) {
		std::pair<std::shared_ptr<Player>, std::shared_ptr<std::vector<std::string>>> pair = std::make_pair(player, std::make_shared<std::vector<string>>());
		this->previousCommands->insert(pair);
	}

	this->previousCommands->at(player)->push_back(input);
}

std::vector<std::string> InputHandler::splitOnSpace(std::string input) {
	std::stringstream ss(input);
	std::istream_iterator<std::string> begin(ss);
	std::istream_iterator<std::string> end;
	std::vector<std::string> params(begin, end);
	return params;
}

void InputHandler::setGame(std::shared_ptr<Game> game) {
	this->game = game;
}

std::shared_ptr<Game> InputHandler::getGame() {
	return this->game;
}

InputHandler::~InputHandler()
{
}
