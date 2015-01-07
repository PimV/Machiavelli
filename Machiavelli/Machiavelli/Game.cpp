#include "Game.h"
#include "Server.h"


Game::Game()
{
	this->loader = std::make_unique<Loader>();
	rounds = 0;
	turns = 0;

	this->characterOrderDeck = loader->loadCharacterCards(std::make_shared<Deck<std::shared_ptr<CharacterCard>>>());

	this->started = false;
}

void Game::startGame() {
	//Set first player to king
	this->player1->setKing(true);

	//Build and shuffle buildingDeck
	this->buildingDeck = loader->loadBuildingCards(std::make_shared<Deck<std::shared_ptr<BuildingCard>>>());
	this->buildingDeck->shuffle();

	//Officially start game (input flag)
	this->startRound();
	this->started = true;
}

void Game::startRound() {
	Server::Instance().broadcast("Ronde: " + std::to_string(rounds));

	//Load initial decks
	this->characterDeck = loader->loadCharacterCards(std::make_shared<Deck<std::shared_ptr<CharacterCard>>>());

	this->characterDeck->shuffle();

	std::shared_ptr<CharacterSelectionTurn> startingTurn = std::make_shared<CharacterSelectionTurn>();
	this->turn = startingTurn;

	//Check for King Card
	if (this->player1->hasCharacterCardByCharacter(Characters::Koning)) {
		this->player1->setKing(true);
		this->player2->setKing(false);
	}
	else if (this->player2->hasCharacterCardByCharacter(Characters::Koning)) {
		this->player1->setKing(false);
		this->player2->setKing(true);
	}

	//Set turn to player with king card (or do nothing if card not found)
	if (this->player1->playerIsKing()) {
		this->turn->setPlayer(player1);
	}
	else if (this->player2->playerIsKing()) {
		this->turn->setPlayer(player2);
	}

	//Show player 1's pre-round options
	if (rounds == 0) {
		this->showCharacterDeckOptions(this->turn->getPlayer());
	}

	rounds++;
	currentCharacterCardIndex = 0;
}

#pragma region Character Card Selection
void Game::pickCharacterCard(std::shared_ptr<Player> player, int index) {
	//Validate stuff
	if (!this->correctPlayerTurn(player)) {
		player->getClient()->write("U bent nog niet aan de beurt. Wacht totdat de andere speler zijn/haar beurt over is.\r\n");
		return;
	}

	if (this->turn->getType() != Turns::CHAR_SELECT) {
		player->getClient()->write("U kunt momenteel geen kaarten meer pakken.\r\n");
		return;
	}

	if (!this->validateCharacterCardInput(index)) {
		player->getClient()->write("U heeft een ongeldig nummer ingevoerd. Probeer het opnieuw.\r\n");
		return;
	}



	std::shared_ptr<CharacterSelectionTurn> cTurn = std::dynamic_pointer_cast<CharacterSelectionTurn>(turn);
	if (cTurn->hasPickedCard()) {
		player->getClient()->write("U heeft al een kaart gepakt deze beurt.");
		return;
	}

	//Get character card
	std::shared_ptr<CharacterCard> card = this->characterDeck->get(index);

	//Check if player can take card
	if (player->takeCharacterCard(card)) {
		//If true, remove card from deck
		this->characterDeck->remove(index);

		//Notify player
		player->getClient()->write("U heeft de " + card->getCharacterString() + " gepakt.\r\n");

		//Do turn handling
		cTurn->pickCard();
		if (this->checkTurnSwitch()) {
			this->switchTurn();
		}
	}
}

void Game::coverCharacterCard(std::shared_ptr<Player> player, int index) {
	//Validate stuff
	if (!this->correctPlayerTurn(player)) {
		player->getClient()->write("U bent nog niet aan de beurt. Wacht totdat de andere speler zijn/haar beurt over is.\r\n");
		return;
	}

	if (this->turn->getType() != Turns::CHAR_SELECT) {
		player->getClient()->write("U kunt momenteel geen kaarten meer gedekt op tafel leggen.\r\n");
		return;
	}

	if (!this->validateCharacterCardInput(index)) {
		player->getClient()->write("U heeft een ongeldig nummer ingevoerd. Probeer het opnieuw.\r\n");
		return;
	}



	std::shared_ptr<CharacterSelectionTurn> cTurn = std::dynamic_pointer_cast<CharacterSelectionTurn>(turn);
	if (cTurn->hasCoveredCard()) {
		player->getClient()->write("U heeft al een kaart gedekt deze beurt.\r\n");
		return;
	}

	//Get character card
	std::shared_ptr<CharacterCard> card = this->characterDeck->get(index);

	//Remove card from deck
	this->characterDeck->remove(index);

	//Notify player
	player->getClient()->write("U heeft de " + card->getCharacterString() + " gedekt op tafel gelegd.\r\n");

	//Specific turn action
	cTurn->coverCard();

	//Check for turn switch
	if (this->checkTurnSwitch()) {
		this->switchTurn();
	}
}


bool Game::validateCharacterCardInput(int index) {
	if (index >= 0 && index < this->characterDeck->size()) {
		std::cout << this->characterDeck->get(index)->toString() << std::endl;
		return true;
	}
	return false;
}
#pragma endregion

#pragma region Character Card Execution

void Game::callCharacterCard() {
	std::cout << "Before: Current Char Index" << currentCharacterCardIndex << std::endl;
	for (size_t i = currentCharacterCardIndex; i < this->characterOrderDeck->size(); i++) {
		std::shared_ptr<CharacterCard> card = this->characterOrderDeck->get(i);
		if (player1->hasCharacterChard(card)) {
			this->turn->setPlayer(player1);
			player1->setActiveCharacterCard(card);
			currentCharacterCardIndex = i + 1;
			break;
		}
		else if (player2->hasCharacterChard(card)) {
			this->turn->setPlayer(player2);
			player2->setActiveCharacterCard(card);
			currentCharacterCardIndex = i + 1;
			break;
		}
	}
	std::cout << "After: Current Char Index" << currentCharacterCardIndex << std::endl;
	Server::Instance().broadcast(this->turn->getPlayer()->getName() + " is aan de beurt met de " + this->turn->getPlayer()->getActiveCharacterCard()->getCharacterString());

	this->turn->getPlayer()->getClient()->write(this->turn->getPlayer()->getActiveCharacterCard()->possibleActions());
}

void Game::takeGold(std::shared_ptr<Player> player) {
	if (!this->correctPlayerTurn(player)) {
		player->getClient()->write("U bent nog niet aan de beurt. Wacht totdat de andere speler zijn/haar beurt over is.\r\n");
		return;
	}

	if (this->turn->getType() != Turns::CHAR_ACTION) {
		player->getClient()->write("U kunt nog geen goud kiezen, omdat u eerst nog karakterkaarten moet kiezen!\r\n");
		return;
	}
	std::shared_ptr<CharacterActionTurn> caTurn = std::dynamic_pointer_cast<CharacterActionTurn>(turn);
	if (caTurn->tookGold() || caTurn->tookCards()) {
		if (caTurn->tookGold()) {
			player->getClient()->write("U heeft in deze beurt al een keer goud gepakt.\r\n");
			return;
		}
		if (caTurn->tookCards()) {
			player->getClient()->write("U heeft in deze beurt al een keer kaarten gepakt.\r\n");
			return;
		}
		return;
	}

	player->changeGoldBy(2);
	caTurn->takeGold();
	player->getClient()->write("U heeft nu " + std::to_string(player->getGold()) + " goudstukken.\r\n");
}

void Game::takeCards(std::shared_ptr<Player> player) {
	if (!this->correctPlayerTurn(player)) {
		player->getClient()->write("U bent nog niet aan de beurt. Wacht totdat de andere speler zijn/haar beurt over is.\r\n");
		return;
	}

	if (this->turn->getType() != Turns::CHAR_ACTION) {
		player->getClient()->write("U kunt nog geen kaarten kiezen, omdat u eerst nog karakterkaarten moet kiezen!\r\n");
		return;
	}

	std::shared_ptr<CharacterActionTurn> caTurn = std::dynamic_pointer_cast<CharacterActionTurn>(turn);
	if (caTurn->tookGold() || caTurn->tookCards()) {
		if (caTurn->tookGold()) {
			player->getClient()->write("U heeft in deze beurt al een keer goud gepakt.\r\n");
			return;
		}
		if (caTurn->tookCards()) {
			player->getClient()->write("U heeft in deze beurt al een keer kaarten gepakt.\r\n");
			return;
		}
		return;
	}

	for (int i = 0; i < 2; i++) {
		player->addChoosableBuildingCards(this->buildingDeck->pop());
	}
	player->printChoosableBuildingCards();

	caTurn->takeCardsFromDeck();
}

void Game::selectBuildingCard(std::shared_ptr<Player> player, int index) {
	if (!this->correctPlayerTurn(player)) {
		player->getClient()->write("U bent nog niet aan de beurt. Wacht totdat de andere speler zijn/haar beurt over is.\r\n");
		return;
	}

	if (this->turn->getType() != Turns::CHAR_ACTION) {
		player->getClient()->write("U kunt nog geen kaarten kiezen, omdat u eerst nog karakterkaarten moet kiezen!\r\n");
		return;
	}

	std::shared_ptr<CharacterActionTurn> caTurn = std::dynamic_pointer_cast<CharacterActionTurn>(turn);
	if (caTurn->tookGold() || caTurn->tookSingleCard()) {
		if (caTurn->tookGold()) {
			player->getClient()->write("U heeft in deze beurt al een keer goud gepakt.\r\n");
			return;
		}
		if (caTurn->tookSingleCard()) {
			player->getClient()->write("U heeft in deze beurt al een bouwkaart geselecteerd.\r\n");
			return;
		}
		return;
	}

	if (std::shared_ptr<BuildingCard> card = player->getChoosableBuildingCardByIndex(index)) {
		player->addBuildingCard(card);
		player->emptyChoosableBuildingCards();
		caTurn->selectSingleCardFromDeck();
	}


}

#pragma endregion

bool Game::correctPlayerTurn(std::shared_ptr<Player> player) {
	if (this->turn->getPlayer() == player) {
		return true;
	}
	return false;
}

#pragma region Player Handling

std::shared_ptr<Player> Game::addPlayer(std::shared_ptr<Socket> client) {
	if (this->player1 == nullptr) {
		this->player1 = std::make_shared<Player>();
		this->player1->setClient(client);
		return this->player1;
	}
	if (this->player2 == nullptr) {
		this->player2 = std::make_shared<Player>();
		this->player2->setClient(client);
		Server::Instance().broadcast("Beide spelers zijn verbonden. Het spel wordt nu gestart...");
		this->startGame();
		return this->player2;
	}

	return nullptr;
}

void Game::removePlayer(std::shared_ptr<Socket> client) {
	if (this->player2 != nullptr && this->player2->getClient() == client) {
		this->player2.reset();
		std::cout << "Deleting player 2 " << std::endl;
		return;
	}

	if (this->player1 != nullptr && this->player1->getClient() == client) {
		this->player1.reset();
		std::cout << "Deleting player 1 " << std::endl;
		return;
	}
}

std::shared_ptr<Player> Game::getPlayerByClient(std::shared_ptr<Socket> client) {

	if (this->player2 != nullptr && this->player2->getClient() == client) {
		return player2;
	}

	if (this->player1 != nullptr && this->player1->getClient() == client) {
		return player1;
	}

	return nullptr;
}

int Game::getPlayerCount() {
	int playerCount = 0;
	if (this->player1 != nullptr) {
		playerCount++;
	}
	if (this->player2 != nullptr) {
		playerCount++;
	}

	return playerCount;
}

#pragma endregion



#pragma region Turn Handling
void Game::showCharacterDeckOptions(std::shared_ptr<Player> player) {
	player->getClient()->write("Mogelijke opties uit de karakterstapel: \r\n");
	for (size_t i = 0; i < this->characterDeck->size(); i++) {
		player->getClient()->write("[" + std::to_string(i + 1) + "] " + this->characterDeck->get(i)->getCharacterString() + "\r\n");
	}
}

bool Game::checkTurnSwitch() {
	if (this->turn->isOver()) {
		return true;
	}
	return false;
}

void Game::switchTurn() {
	if (this->turn->isOver()) {
		turns++;
		if (this->turn->getPlayer() == player1) {
			this->turn->setPlayer(player2);
			std::cout << "Turn to player2" << std::endl;
		}
		else if (this->turn->getPlayer() == player2) {
			this->turn->setPlayer(player1);
			std::cout << "Turn to player1" << std::endl;
		}

		//Switch turn types
		this->switchTurnTypes();

		//Reset current turn;
		this->turn->resetTurn();


		//Prepare clients for new turn
		this->prepareTurn();

		std::cout << "Server switching turns..." << std::endl;
	}
}

void Game::switchTurnTypes() {
	if (turns % 4 == 0) {
		if (this->turn->getType() == Turns::CHAR_SELECT) {
			//Switch to character action turn
			this->turn = std::make_shared<CharacterActionTurn>();
			std::cout << "turn type switch to action" << std::endl;
		}
		else if (this->turn->getType() == Turns::CHAR_ACTION) {
			this->startRound();
			player1->resetForNextRound();
			player2->resetForNextRound();
		}

	}
}

void Game::prepareTurn() {
	if (this->turn->getType() == Turns::CHAR_SELECT) {
		std::cout << "Showing character deck options to player" << std::endl;
		//this->turn->getPlayer()->getClient()->write("U bent aan de beurt en moet nu één kaart bedekken (dek <kaart_index>) en één kaart pakken (pak <kaart_index>).\r\n");
		this->showCharacterDeckOptions(this->turn->getPlayer());
	}
	else if (this->turn->getType() == Turns::CHAR_ACTION) {
		std::cout << "Asking player what to do with this character" << std::endl;
		this->callCharacterCard();
	}
}

#pragma endregion

std::shared_ptr<Deck<std::shared_ptr<CharacterCard>>> Game::getCharacterDeck() {
	return this->characterDeck;
}

std::shared_ptr<Deck<std::shared_ptr<BuildingCard>>> Game::getBuildingDeck() {
	return this->buildingDeck;
}


bool Game::isStarted() {
	return started;
}



Game::~Game()
{
}
