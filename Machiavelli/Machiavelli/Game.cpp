#include "Game.h"
#include "Server.h"


Game::Game()
{
	this->loader = std::make_unique<Loader>();

	this->buildingDeck = loader->loadBuildingCards(std::make_shared<Deck<std::shared_ptr<BuildingCard>>>());
	this->characterDeck = loader->loadCharacterCards(std::make_shared<Deck<std::shared_ptr<CharacterCard>>>());

	rounds = 0;

	this->started = false;
}

void Game::startGame() {
	//Starting first round;
	rounds++;

	//Set first player to king
	Server::Instance().broadcast("Omdat speler 1 (" + this->player1->getName() + ") als eerste verbonden is, wordt deze speler de koning!");
	this->player1->setKing(true);
	this->player1->takeCharacterCard(this->characterDeck->get(3));
	this->characterDeck->remove(3);

	//Shuffle deck and show first player first card
	Server::Instance().broadcast("Speler 1 (" + this->player1->getName() + ") schudt de kaarten, bekijkt de eerste kaart en legt deze gedekt op tafel.\r\n");
	this->characterDeck->shuffle();
	this->player1->getClient()->write("Het eerste karakter is: ");
	this->player1->getClient()->write(this->characterDeck->pop()->getCharacterString() + "\r\n");

	//Show player 1's pre-round options
	this->showCharacterDeckOptions(player1);

	//Set turn to player 1
	std::shared_ptr<CharacterSelectionTurn> startingTurn = std::make_shared<CharacterSelectionTurn>();
	startingTurn->coverCard();
	this->turn = startingTurn;
	this->turn->setPlayer(player1);


	//Officially start game (input flag)
	this->started = true;
}

void Game::pickCharacterCard(std::shared_ptr<Player> player, int index) {
	//Validate stuff
	if (!this->validateCharacterCardInput(index)) {
		player->getClient()->write("U heeft een ongeldig nummer ingevoerd. Probeer het opnieuw.\r\n");
		return;
	}

	if (!this->correctPlayerTurn(player)) {
		player->getClient()->write("U bent nog niet aan de beurt. Wacht totdat de andere speler zijn/haar beurt over is.\r\n");
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
	if (!this->validateCharacterCardInput(index)) {
		player->getClient()->write("U heeft een ongeldig nummer ingevoerd. Probeer het opnieuw.\r\n");
		return;
	}

	if (!this->correctPlayerTurn(player)) {
		player->getClient()->write("U bent nog niet aan de beurt. Wacht totdat de andere speler zijn/haar beurt over is.\r\n");
		return;
	}

	std::shared_ptr<CharacterSelectionTurn> cTurn = std::dynamic_pointer_cast<CharacterSelectionTurn>(turn);
	if (cTurn->hasCoveredCard()) {
		player->getClient()->write("U heeft al een kaart gepakt deze beurt.");
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
		if (this->turn->getPlayer() == player1) {
			this->turn->setPlayer(player2);
		}
		else if (this->turn->getPlayer() == player2) {
			this->turn->setPlayer(player1);
		}
		this->turn->resetTurn();
		std::cout << "Server switching turns..." << std::endl;
	}
	else {
		std::cout << "Turn not over yet, continue" << std::endl;
	}
}

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
