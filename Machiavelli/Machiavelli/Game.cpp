#include "Game.h"
#include "Server.h"


Game::Game()
{
	this->loader = std::make_unique<Loader>();

	this->buildingDeck = loader->loadBuildingCards(std::make_shared<Deck<std::shared_ptr<BuildingCard>>>());
	this->characterDeck = loader->loadCharacterCards(std::make_shared<Deck<std::shared_ptr<CharacterCard>>>());
}

std::shared_ptr<Player> Game::addPlayer(std::shared_ptr<Socket> client) {
	if (this->player1 == nullptr) {
		this->player1 = std::make_shared<Player>();
		this->player1->setClient(client);
		std::cout << "Created player 1" << std::endl;
		return this->player1;
	}
	if (this->player2 == nullptr) {
		this->player2 = std::make_shared<Player>();
		this->player2->setClient(client);
		std::cout << "Created player 2" << std::endl;

		return this->player2;
	}

	return nullptr;
}

void Game::removePlayer(std::shared_ptr<Socket> client) {
	
	if (this->player2 != nullptr && this->player2->getClient() == client) {
		this->player2.reset();
		std::cout << "Deleting player 2 " << std::endl;
	}

	if (this->player1 != nullptr && this->player1->getClient() == client) {
		this->player1.reset();
		std::cout << "Deleting player 1 " << std::endl;
	}

	//client->close();
	//client.reset();



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

std::shared_ptr<Deck<std::shared_ptr<CharacterCard>>> Game::getCharacterDeck() {
	return this->characterDeck;
}

std::shared_ptr<Deck<std::shared_ptr<BuildingCard>>> Game::getBuildingDeck() {
	return this->buildingDeck;
}




Game::~Game()
{
}
