#include "InputHandler.h"
#include <string>
#include <sstream>
#include <algorithm>
#include "Server.h"

#include "Game.h"

InputHandler::InputHandler()
{
	previousCommands = std::make_shared<std::map<std::shared_ptr<Player>, std::shared_ptr<std::vector<std::string>>>>();

	globalCmds.insert(std::pair<std::string, std::string>("broadcast", "broadcast"));
	globalCmds.insert(std::pair<std::string, std::string>("historie", "historie"));
	globalCmds.insert(std::pair<std::string, std::string>("naam", "naam <nieuwe_naam>"));
	globalCmds.insert(std::pair<std::string, std::string>("help", "help"));
	globalCmds.insert(std::pair<std::string, std::string>("disconnect", "disconnect"));
	globalCmds.insert(std::pair<std::string, std::string>("quit", "quit"));

	tiCmds.insert(std::pair<std::string, std::string>("bekijk_karakterkaarten", "bekijk_karakterkaarten"));
	tiCmds.insert(std::pair<std::string, std::string>("bekijk_bouwkaarten", "bekijk_bouwkaarten"));
	tiCmds.insert(std::pair<std::string, std::string>("bekijk_gebouwen", "bekijk_gebouwen"));
	tiCmds.insert(std::pair<std::string, std::string>("bekijk_gebouwen_tegenstander", "bekijk_gebouwen_tegenstander"));
	tiCmds.insert(std::pair<std::string, std::string>("bekijk_goud", "bekijk_goud"));
	tiCmds.insert(std::pair<std::string, std::string>("aan_de_beurt", "aan_de_beurt"));

	//Character selection commands
	gameCmds.insert(std::pair<std::string, std::string>("pak", "pak <index>"));
	gameCmds.insert(std::pair<std::string, std::string>("dek", "dek <index>"));
	gameCmds.insert(std::pair<std::string, std::string>("bekijk_alle_karakterkaarten", "bekijk_alle_karakterkaarten"));

	//Building card commands
	gameCmds.insert(std::pair<std::string, std::string>("pak_bouwkaarten", "pak_bouwkaarten"));
	gameCmds.insert(std::pair<std::string, std::string>("selecteer_bouwkaart", "selecteer_bouwkaart <index>"));
	gameCmds.insert(std::pair<std::string, std::string>("bouw_bouwkaart", "bouw_bouwkaart <index>"));

	/* Moordenaar */
	gameCmds.insert(std::pair<std::string, std::string>("vermoord", "vermoord <index>"));
	gameCmds.insert(std::pair<std::string, std::string>("vermoord_opties", "vermoord_opties"));

	/* Dief */
	gameCmds.insert(std::pair<std::string, std::string>("besteel", "besteel <index>"));
	gameCmds.insert(std::pair<std::string, std::string>("besteel_opties", "besteel_opties"));

	/* Magiër */
	gameCmds.insert(std::pair<std::string, std::string>("ruil_hand", "ruil_hand"));
	gameCmds.insert(std::pair<std::string, std::string>("ruil_kaarten", "ruil_kaarten <kaartindex1,kaartindex2,kaartindex3>"));

	/* Condottiere */
	gameCmds.insert(std::pair<std::string, std::string>("verwijder_gebouw", "verwijder_gebouw <index>"));
	gameCmds.insert(std::pair<std::string, std::string>("verwijder_opties", "verwijder_opties"));

	/* Player Stats */
	gameCmds.insert(std::pair<std::string, std::string>("bekijk_karakterkaarten", "bekijk_karakterkaarten"));
	gameCmds.insert(std::pair<std::string, std::string>("bekijk_bouwkaarten", "bekijk_bouwkaarten"));
	gameCmds.insert(std::pair<std::string, std::string>("bekijk_gebouwen", "bekijk_gebouwen"));
	gameCmds.insert(std::pair<std::string, std::string>("bekijk_gebouwen_tegenstander", "bekijk_gebouwen_tegenstander"));
	gameCmds.insert(std::pair<std::string, std::string>("bekijk_goud", "bekijk_goud"));

	/* Building special moves */
	gameCmds.insert(std::pair<std::string, std::string>("werkplaats_special", "werkplaats_special"));
	gameCmds.insert(std::pair<std::string, std::string>("laboratorium_special", "laboratorium_special <index>"));

	//Take gold command
	gameCmds.insert(std::pair<std::string, std::string>("pak_goud", "pak_goud"));

	//Turn commands
	gameCmds.insert(std::pair<std::string, std::string>("beurt_over", "beurt_over"));
	gameCmds.insert(std::pair<std::string, std::string>("aan_de_beurt", "aan_de_beurt"));

	//Help command	
	gameCmds.insert(std::pair<std::string, std::string>("acties", "acties"));
}

bool InputHandler::isGlobalCommand(std::string command) {
	if (globalCmds.count(command)) {
		return true;
	}
	return false;
}

bool InputHandler::isGameCommand(std::string command) {

	if (gameCmds.count(command)) {
		return true;
	}
	return false;
}

bool InputHandler::isTurnIndependentCommand(std::string command) {
	if (tiCmds.count(command)) {
		return true;
	}
	return false;
}

bool InputHandler::handleInput(std::string input, std::shared_ptr<Player> player) {
	if (input.length() > 0) {
		try {
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
		catch (const exception& ex) {
			player->getClient()->write("Uw input kon niet verwerkt worden.\r\n");
			std::cout << "Could not process input from Client [:" << player->getName() << std::endl;
		}

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
			player->getClient()->write("Uw gebouwen: \r\n");
			player->getClient()->write(player->printBuildings());
		}
		else if (params[0] == "bekijk_gebouwen_tegenstander") {
			this->game->printOpponentBuildings(player);
		}
		else if (params[0] == "bekijk_goud") {
			this->checkGold(params, player);
		}
		else if (params[0] == "aan_de_beurt") {
			this->game->printOnTurn(player);
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
	else if (params[0] == "verwijder_gebouw") {
		this->remove(params, player);
	}
	else if (params[0] == "verwijder_opties") {
		this->removeOptions(params, player);
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
	else if (params[0] == "acties") {
		this->game->printPossibleActions(player);
	}
	else if (params[0] == "werkplaats_special") {
		this->game->workshopSpecial(player);
	}
	else if (params[0] == "laboratorium_special") {
		this->labSpecial(params, player);
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
	else if (params[0] == "help") {
		player->getClient()->write("ALLE commando's (ook momenteel ongeldige commando's!): \r\n");
		player->getClient()->write("GAME COMMANDS: \r\n");
		for (auto iter : gameCmds) {
			player->getClient()->write("\t - " + iter.second + "\r\n");
		}
		player->getClient()->write("GLOBAL COMMANDS: \r\n");

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
		int index = stringToInt(params[1]);
		this->game->coverCharacterCard(player, index);
	}
	else {
		player->getClient()->write("Incorrect gebruik. Voorbeeld: dek 1\r\n");
	}
}

void InputHandler::pickCard(std::vector<std::string> params, std::shared_ptr<Player> player) {
	if (params.size() > 1) {
		int index = stringToInt(params[1]);
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
		int index = stringToInt(params[1]);
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
		int index = stringToInt(params[1]);
		this->game->constructBuilding(player, index);
	}
	else {
		player->getClient()->write("Incorrect gebruik. Voorbeeld: selecteer_bouwkaart 1\r\n");
	}
}

#pragma endregion

#pragma region Moordenaar
void InputHandler::murder(std::vector<std::string> params, std::shared_ptr<Player> player) {
	if (this->game->getTurn()->getType() != Turns::CHAR_ACTION) {
		player->getClient()->write("Deze actie kan nog niet uitgevoerd worden, omdat het spel in een andere fase zit.\r\n");
		return;
	}
	if (this->game->correctCharacterTurn(player, Characters::Moordenaar)) {
		if (params.size() > 1) {
			int index = stringToInt(params[1]);
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
			int index = stringToInt(params[1]);
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

#pragma region Condottiere

void InputHandler::remove(std::vector<std::string> params, std::shared_ptr<Player> player) {
	if (this->game->getTurn()->getType() != Turns::CHAR_ACTION) {
		player->getClient()->write("Deze actie kan nog niet uitgevoerd worden, omdat het spel in een andere fase zit.\r\n");
		return;
	}
	if (this->game->correctCharacterTurn(player, Characters::Condottiere)) {
		if (params.size() > 1) {
			int index = stringToInt(params[1]);
			this->game->removeBuilding(player, index);
		}
		else {
			player->getClient()->write("Incorrect gebruik. Voorbeeld: besteel 1\r\n");
		}
	}
	else {
		player->getClient()->write("U kunt dit commando niet uitvoeren met dit karakter\r\n");
	}
}

void InputHandler::removeOptions(std::vector<std::string> params, std::shared_ptr<Player> player) {
	if (this->game->getTurn()->getType() != Turns::CHAR_ACTION) {
		player->getClient()->write("Deze actie kan nog niet uitgevoerd worden, omdat het spel in een andere fase zit.\r\n");
		return;
	}
	if (this->game->correctCharacterTurn(player, Characters::Dief)) {
		this->game->printOpponentBuildings(player);
	}
	else {
		player->getClient()->write("U kunt dit commando niet uitvoeren met dit karakter\r\n");
	}

}

#pragma endregion
void InputHandler::labSpecial(std::vector<std::string> params, std::shared_ptr<Player> player) {
	if (params.size() > 1) {
		int index = stringToInt(params[1]);
		this->game->labSpecial(player, index);
	}
	else {
		player->getClient()->write("Incorrect gebruik. Voorbeeld: laboratorium_special 1\r\n");
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

std::vector<std::string> InputHandler::splitOnComma(std::string input) {
	std::istringstream ss(input);
	std::string token;
	std::vector<std::string> output;

	while (std::getline(ss, token, ',')) {
		output.push_back(token);
	}

	return output;
}

int InputHandler::stringToInt(std::string input) {
	int retVal = -1;
	try {
		retVal = stoi(input);
		retVal = retVal - 1;
	}
	catch (...) {

	}
	return retVal;
}

void InputHandler::setGame(std::shared_ptr<Game> game) {
	this->game = game;
}

std::shared_ptr<Game> InputHandler::getGame() {
	return this->game;
}


InputHandler::~InputHandler()
{
	OutputDebugStringW(L"My output string.\r\n");
}
