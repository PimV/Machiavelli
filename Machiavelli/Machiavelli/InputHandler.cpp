#include "InputHandler.h"
#include <string>
#include <sstream>
#include <algorithm>
#include "Server.h"

#include "Game.h"

InputHandler::InputHandler()
{
	previousCommands = std::make_shared<std::map<std::shared_ptr<Player>, std::shared_ptr<std::vector<std::string>>>>();

	turnIndependentCommands.push_back("bekijk_karakterkaarten");
	turnIndependentCommands.push_back("bekijk_bouwkaarten");
	turnIndependentCommands.push_back("bekijk_gebouwen");
	turnIndependentCommands.push_back("bekijk_goud");

	//Character selection commands
	gameCommands.push_back("pak");
	gameCommands.push_back("dek");
	gameCommands.push_back("bekijk_alle_karakterkaarten");

	//Building card commands
	gameCommands.push_back("pak_bouwkaarten");
	gameCommands.push_back("selecteer_bouwkaart");
	gameCommands.push_back("bouw_bouwkaart");

	/* Moordenaar */
	gameCommands.push_back("vermoord");
	gameCommands.push_back("vermoord_opties");

	/* Dief */
	gameCommands.push_back("besteel");
	gameCommands.push_back("besteel_opties");

	/* Magiër */
	gameCommands.push_back("ruil_hand");
	gameCommands.push_back("ruil_kaarten");

	/* Player Stats */
	gameCommands.push_back("bekijk_karakterkaarten");
	gameCommands.push_back("bekijk_bouwkaarten");
	gameCommands.push_back("bekijk_gebouwen");
	gameCommands.push_back("bekijk_goud");

	//Take gold command
	gameCommands.push_back("pak_goud");

	//Turn over command
	gameCommands.push_back("beurt_over");

	//Help command
	gameCommands.push_back("help");
}

bool InputHandler::isGameCommand(std::string command) {
	if (std::find(gameCommands.begin(), gameCommands.end(), command) != gameCommands.end())	{
		return true;
	}
	return false;
}

bool InputHandler::isTurnIndependentCommand(std::string command) {
	if (std::find(turnIndependentCommands.begin(), turnIndependentCommands.end(), command) != turnIndependentCommands.end())	{
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

		this->updatePreviousCommands(input, player);
	}
	return true;
}

void InputHandler::handleGameCommand(std::vector<std::string> params, std::shared_ptr<Player> player) {
	//Check if game is started for game commands
	if (!this->game->isStarted()) {
		player->getClient()->write("Het spel is nog niet begonnen!\r\n");
		return;
	}

	//Handle turn independent commands
	if (isTurnIndependentCommand(params[0])) {
		if (params[0] == "bekijk_bouwkaarten") {
			player->printBuildingCards();
		}
		else if (params[0] == "bekijk_karakterkaarten") {

		}
		else if (params[0] == "bekijk_gebouwen") {
			this->checkBuildings(params, player);
		}
		else if (params[0] == "bekijk_goud") {
			this->checkGold(params, player);
		}

		return;
	}

	//Check if correct player has turn
	if (!this->game->correctPlayerTurn(player)) {
		player->getClient()->write("U bent nog niet aan de beurt. Wacht totdat de andere speler zijn/haar beurt over is.\r\n");
		return;
	}

	//Handle turn dependent commands
	if (params[0] == "beurt_over") {
		this->finishTurn(params, player);
	}
	else if (params[0] == "pak_goud") {
		this->game->takeGold(player);
	}
	else if (params[0] == "pak_bouwkaarten") {
		this->game->takeCards(player);
	}
	else if (params[0] == "selecteer_bouwkaart") {
		this->selectBuildCard(params, player);
	}
	else if (params[0] == "bouw_bouwkaart") {
		this->build(params, player);
	}
	else if (params[0] == "pak") {
		this->pickCard(params, player);
	}
	else if (params[0] == "dek") {
		this->coverCard(params, player);
	}
	else if (params[0] == "vermoord") {
		this->murder(params, player);
	}
	else if (params[0] == "vermoord_opties") {
		this->murderOptions(params, player);
	}
	else if (params[0] == "besteel") {
		this->pickpocket(params, player);
	}
	else if (params[0] == "besteel_opties") {
		this->pickpocketOptions(params, player);
	}
	else if (params[0] == "bekijk_alle_karakterkaarten") {
		this->game->showCharacterDeckOptions(player);
	}
	else if (params[0] == "ruil_hand") {
		this->swapHand(params, player);
	}
	else if (params[0] == "ruil_kaarten") {
		this->swapCards(params, player);
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
#pragma region Turns
void InputHandler::finishTurn(std::vector<std::string> params, std::shared_ptr<Player> player) {
	if (this->game->checkTurnSwitch()) {
		this->game->switchTurn();
	}
	else {
		player->getClient()->write("U moet minimaal 2 goud óf 2 bouwkaarten pakken per beurt.\r\n");
	}
}

#pragma endregion

#pragma region Character Selection
void InputHandler::coverCard(std::vector<std::string> params, std::shared_ptr<Player> player) {
	if (params.size() > 1) {
		int index = stoi(params[1]);
		index = index - 1; // correct for + 1 visible to user
		this->game->coverCharacterCard(player, index);
	}
	else {
		player->getClient()->write("Incorrect gebruik. Voorbeeld: dek 1\r\n");
	}
}

void InputHandler::pickCard(std::vector<std::string> params, std::shared_ptr<Player> player) {
	if (params.size() > 1) {
		int index = stoi(params[1]);
		index = index - 1; // correct for + 1 visible to user
		this->game->pickCharacterCard(player, index);
	}
	else {
		player->getClient()->write("Incorrect gebruik. Voorbeeld: pak 1\r\n");
	}
}

#pragma endregion

#pragma region Character Action
void InputHandler::checkGold(std::vector<std::string> params, std::shared_ptr<Player> player) {
	player->getClient()->write("U heeft " + std::to_string(player->getGold()) + " goudstukken.\r\n");
}

void InputHandler::selectBuildCard(std::vector<std::string> params, std::shared_ptr<Player> player) {
	if (params.size() > 1) {
		int index = stoi(params[1]);
		index = index - 1; // correct for + 1 visible to user
		this->game->selectBuildingCard(player, index);
	}
	else {
		player->getClient()->write("Incorrect gebruik. Voorbeeld: selecteer_bouwkaart 1\r\n");
	}
}

void InputHandler::build(std::vector<std::string> params, std::shared_ptr<Player> player) {
	if (this->game->getTurn()->getType() != Turns::CHAR_ACTION) {
		player->getClient()->write("Deze actie kan nog niet uitgevoerd worden, omdat het spel in een andere fase zit.\r\n");
		return;
	}
	if (params.size() > 1) {
		int index = stoi(params[1]);
		index = index - 1; // correct for + 1 visible to user
		player->constructBuildingCard(index);


	}
	else {
		player->getClient()->write("Incorrect gebruik. Voorbeeld: selecteer_bouwkaart 1\r\n");
	}
}

void InputHandler::checkBuildings(std::vector<std::string> params, std::shared_ptr<Player> player) {
	player->printBuildings();
}

#pragma endregion

#pragma region Moordenaar
void InputHandler::murder(std::vector<std::string> params, std::shared_ptr<Player> player) {
	if (this->game->getTurn()->getType() != Turns::CHAR_ACTION) {
		player->getClient()->write("Deze actie kan nog niet uitgevoerd worden, omdat het spel in een andere fase zit.\r\n");
		return;
	}
	if (this->game->correctCharacterTurn(player, Characters::Dief)) {
		if (params.size() > 1) {
			int index = stoi(params[1]);
			index = index - 1; // correct for + 1 visible to user
			this->game->murderCharacter(player, index);
		}
		else {
			player->getClient()->write("Incorrect gebruik. Voorbeeld: vermoord 2\r\n");
		}
	}
	else {
		player->getClient()->write("U kunt dit commando niet uitvoeren met dit karakter\r\n");
	}
}

void InputHandler::murderOptions(std::vector<std::string> params, std::shared_ptr<Player> player) {
	if (this->game->getTurn()->getType() != Turns::CHAR_ACTION) {
		player->getClient()->write("Deze actie kan nog niet uitgevoerd worden, omdat het spel in een andere fase zit.\r\n");
		return;
	}
	if (player->hasCharacterCardByCharacter(Characters::Moordenaar) && this->game->getTurn()->getPlayer()->getActiveCharacterCard()->getCharacter() == Characters::Moordenaar) {
		this->game->printCharacterOrderCard(player);
	}
	else {
		player->getClient()->write("U kunt dit commando niet uitvoeren met dit karakter\r\n");
	}
}
#pragma endregion

#pragma region Dief

void InputHandler::pickpocket(std::vector<std::string> params, std::shared_ptr<Player> player) {
	if (this->game->getTurn()->getType() != Turns::CHAR_ACTION) {
		player->getClient()->write("Deze actie kan nog niet uitgevoerd worden, omdat het spel in een andere fase zit.\r\n");
		return;
	}
	if (this->game->correctCharacterTurn(player, Characters::Dief)) {
		if (params.size() > 1) {
			int index = stoi(params[1]);
			index = index - 1; // correct for + 1 visible to user
			this->game->pickpocketCharacter(player, index);
		}
		else {
			player->getClient()->write("Incorrect gebruik. Voorbeeld: besteel 1\r\n");
		}
	}
	else {
		player->getClient()->write("U kunt dit commando niet uitvoeren met dit karakter\r\n");
	}
}

void InputHandler::pickpocketOptions(std::vector<std::string> params, std::shared_ptr<Player> player) {
	if (this->game->getTurn()->getType() != Turns::CHAR_ACTION) {
		player->getClient()->write("Deze actie kan nog niet uitgevoerd worden, omdat het spel in een andere fase zit.\r\n");
		return;
	}
	if (this->game->correctCharacterTurn(player, Characters::Dief)) {
		this->game->printCharacterOrderCard(player);
	}
	else {
		player->getClient()->write("U kunt dit commando niet uitvoeren met dit karakter\r\n");
	}
}

#pragma endregion

#pragma region Magiër
void InputHandler::swapHand(std::vector<std::string> params, std::shared_ptr<Player> player) {
	if (this->game->getTurn()->getType() != Turns::CHAR_ACTION) {
		player->getClient()->write("Deze actie kan nog niet uitgevoerd worden, omdat het spel in een andere fase zit.\r\n");
		return;
	}
	if (this->game->correctCharacterTurn(player, Characters::Magier)) {
		this->game->swapHands(player);
	}
	else {
		player->getClient()->write("U kunt dit commando niet uitvoeren met dit karakter.\r\n");
	}

}

void InputHandler::swapCards(std::vector<std::string> params, std::shared_ptr<Player> player) {
	if (this->game->getTurn()->getType() != Turns::CHAR_ACTION) {
		player->getClient()->write("Deze actie kan nog niet uitgevoerd worden, omdat het spel in een andere fase zit.\r\n");
		return;
	}
	if (this->game->correctCharacterTurn(player, Characters::Magier)) {
		if (params.size() > 1) {
			std::vector<std::string> stringIndices = this->splitOnComma(params[1]);
			std::vector<int> indices;
			std::string invalids = "Ongeldig(e) argument(en): ";
			bool invalidArgumentFound = false;
			for (size_t i = 0; i < stringIndices.size(); i++) {
				if (stringIndices[i].length() == 0) {
					continue;
				}
				try {
					indices.push_back(stoi(stringIndices[i]) - 1);
				}
				catch (exception &ex) {
					invalidArgumentFound = true;
					invalids.append(stringIndices[i] + ", ");
				}
			}
			if (invalidArgumentFound) {
				player->getClient()->write(invalids + "\r\n");
			}
			else {
				std::cout << indices.size() << std::endl;
				std::sort(indices.begin(), indices.end());
				this->game->swapCards(player, indices);
				//Continue
			}
		}
		else {
			player->getClient()->write("Incorrect gebruik. Voorbeeld: ruil_kaarten 1,2,3\r\n");
		}
	}
	else {
		player->getClient()->write("U kunt dit commando niet uitvoeren met dit karakter.\r\n");
	}
}

#pragma endregion

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

std::vector<std::string> InputHandler::splitOnComma(std::string input) {
	std::istringstream ss(input);
	std::string token;
	std::vector<std::string> output;

	while (std::getline(ss, token, ',')) {
		output.push_back(token);
	}

	return output;
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
