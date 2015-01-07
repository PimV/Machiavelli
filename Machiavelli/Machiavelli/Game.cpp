#include "Game.h"
#include "Server.h"


Game::Game()
{
	this->loader = std::make_unique<Loader>();
	rounds = 0;
	turns = 0;

	//this->characterOrderDeck = loader->loadCharacterCards(std::make_shared<Deck<std::shared_ptr<CharacterCard>>>());

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
		player->getClient()->write("U heeft al een kaart gepakt deze beurt.\r\n");
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
	for (size_t i = currentCharacterCardIndex; i < this->characterOrderDeck->size(); i++) {
		std::shared_ptr<CharacterCard> card = this->characterOrderDeck->get(i);

		if (player1->hasCharacterChard(card)) {
			if (card->isMurdered()) {
				std::cout << "Skipping " << card->getCharacterString() << " cause murdered" << std::endl;
				turns++;
				currentCharacterCardIndex = i + 1;
				continue;
			}

			if (card->isPickpocketed()) {
				Server::Instance().broadcast(player2->getName() + " heeft de Dief achter een karakter van " + player1->getName() + " gestuurd.");
				Server::Instance().broadcast(player2->getName() + " krijgt daarom al het goud van " + player1->getName() + "(" + std::to_string(player1->getGold()) + ")");
				player2->changeGoldBy(player1->getGold());
				player1->changeGoldBy(-player1->getGold());
			}
			this->turn->setPlayer(player1);
			player1->setActiveCharacterCard(card);
			currentCharacterCardIndex = i + 1;
			break;
		}
		else if (player2->hasCharacterChard(card)) {
			if (card->isMurdered()) {
				std::cout << "Skipping " << card->getCharacterString() << " cause murdered" << std::endl;
				turns++;
				currentCharacterCardIndex = i + 1;
				continue;
			}
			if (card->isPickpocketed()) {
				Server::Instance().broadcast(player1->getName() + " heeft de Dief achter een karakter van " + player2->getName() + " gestuurd.");
				Server::Instance().broadcast(player1->getName() + " krijgt daarom al het goud van " + player2->getName() + "(" + std::to_string(player2->getGold()) + ")");
				player1->changeGoldBy(player2->getGold());
				player2->changeGoldBy(-player2->getGold());
			}
			this->turn->setPlayer(player2);
			player2->setActiveCharacterCard(card);
			currentCharacterCardIndex = i + 1;
			break;
		}
	}
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

		//Notify player
		player->getClient()->write("U heeft de volgende bouwkaart gepakt: \r\n");
		player->getClient()->write(card->toString() + "\r\n");
	}
}

void Game::murderCharacter(std::shared_ptr<Player> player, int index) {
	std::shared_ptr<CharacterActionTurn> caTurn = std::dynamic_pointer_cast<CharacterActionTurn>(turn);
	if (caTurn->didSpecial()) {
		player->getClient()->write("U heeft al uw speciale karaktereigenschap gebruikt deze beurt!\r\n");
		return;
	}

	if (index >= 0 && index < this->characterOrderDeck->size()) {
		std::shared_ptr<CharacterCard> card = this->characterOrderDeck->get(index);

		if (player->hasCharacterCardByCharacter(card->getCharacter())) {
			player->getClient()->write("U kunt niet uw eigen karakter vermoorden!\r\n");
			return;
		}

		card->setMurdered(true);
		Server::Instance().broadcast(player->getName() + " heeft de " + card->getCharacterString() + " vermoord.");
		caTurn->doSpecial();

	}
	else {
		player->getClient()->write("Ongeldige index geselecteerd. \r\n");
	}
}

void Game::pickpocketCharacter(std::shared_ptr<Player> player, int index) {
	std::shared_ptr<CharacterActionTurn> caTurn = std::dynamic_pointer_cast<CharacterActionTurn>(turn);
	if (caTurn->didSpecial()) {
		player->getClient()->write("U heeft al uw speciale karaktereigenschap gebruikt deze beurt!\r\n");
		return;
	}

	if (index >= 0 && index < this->characterOrderDeck->size()) {
		std::shared_ptr<CharacterCard> card = this->characterOrderDeck->get(index);

		if (player->hasCharacterCardByCharacter(card->getCharacter())) {
			player->getClient()->write("U kunt niet uw eigen karakter bestelen!\r\n");
			return;
		}

		card->setPickpocketed(true);
		player->getClient()->write("U heeft de Dief achter de " + card->getCharacterString() + " gestuurd.\r\n");
		caTurn->doSpecial();
	}
	else {
		player->getClient()->write("Ongeldige index geselecteerd. \r\n");
	}
}

void Game::swapHands(std::shared_ptr<Player> player) {
	std::shared_ptr<CharacterActionTurn> caTurn = std::dynamic_pointer_cast<CharacterActionTurn>(turn);
	if (caTurn->didSpecial()) {
		player->getClient()->write("U heeft al uw speciale karaktereigenschap gebruikt deze beurt!\r\n");
		return;
	}

	if (player == player1) {
		std::vector<std::shared_ptr<BuildingCard>> temp = player2->getBuildingCards();
		player2->setBuildingCards(player->getBuildingCards());
		player1->setBuildingCards(temp);
		Server::Instance().broadcast(player1->getName() + " heeft zijn handkaarten geruild met " + player2->getName());
	}
	else if (player == player2) {
		std::vector<std::shared_ptr<BuildingCard>> temp = player1->getBuildingCards();
		player1->setBuildingCards(player->getBuildingCards());
		player2->setBuildingCards(temp);
		Server::Instance().broadcast(player2->getName() + " heeft zijn handkaarten geruild met " + player1->getName());

	}
	caTurn->doSpecial();
}

void Game::swapCards(std::shared_ptr<Player> player, std::vector<int> indices) {
	std::shared_ptr<CharacterActionTurn> caTurn = std::dynamic_pointer_cast<CharacterActionTurn>(turn);
	if (caTurn->didSpecial()) {
		player->getClient()->write("U heeft al uw speciale karaktereigenschap gebruikt deze beurt!\r\n");
		return;
	}

	if (indices.size() > 0) {
		std::string cardsRemoved = "U heeft deze kaart(en) weggelegd: \r\n";
		std::string cardsReceived = "U heeft deze kaart(en) teruggekregen: \r\n";

		bool deckEmpty = false;
		int cardsSwapped = 0;
		for (unsigned i = indices.size() - 1; indices.size() > i; --i) {
			if (this->buildingDeck->size() == 0) {
				deckEmpty = true;
				break;
			}
			int index = indices[i];
			if (index >= 0 && index < player->getBuildingCards().size()) {
				std::shared_ptr<BuildingCard> card = player->getBuildingCards().at(index);
				std::shared_ptr<BuildingCard> newCard = this->buildingDeck->pop();
				cardsRemoved.append("\t - " + card->toString() + "\r\n");
				cardsReceived.append("\t - " + newCard->toString() + "\r\n");
				player->removeBuildingCard(card);
				player->addBuildingCard(newCard);
				cardsSwapped++;
			}
			else {
				std::cout << "index out of bounds" << std::endl;
			}
		}
		if (cardsSwapped == 0) {
			player->getClient()->write("U heeft geen kaarten kunnen wisselen. Probeer het opnieuw. \r\n");
			return;
		}
		player->getClient()->write(cardsRemoved + "\r\n");
		player->getClient()->write(cardsReceived + "\r\n");
		player->getClient()->write("U heeft " + std::to_string(cardsSwapped) + " kaarten succesvol omgeruild.\r\n");
		if (deckEmpty) {
			player->getClient()->write("U heeft " + std::to_string(indices.size() - cardsSwapped) + " kaart(en) niet succesvol om kunnen ruilen, omdat de stapel leeg was.\r\n");
		}
		caTurn->doSpecial();
		
	}
}

#pragma endregion

bool Game::correctPlayerTurn(std::shared_ptr<Player> player) {
	if (this->turn->getPlayer() == player) {
		return true;
	}
	return false;
}

bool Game::correctCharacterTurn(std::shared_ptr<Player> player, Characters character) {
	if (player->hasCharacterCardByCharacter(character) && getTurn()->getPlayer()->getActiveCharacterCard()->getCharacter() == character) {
		return true;
	}
	return false;
}

#pragma region Player Handling

std::shared_ptr<Player> Game::addPlayer(std::shared_ptr<Socket> client) {
	if (this->player1 == nullptr) {
		this->player1 = std::make_shared<Player>();
		this->player1->setClient(client);
		this->player1->setName("Player 1");
		return this->player1;
	}
	if (this->player2 == nullptr) {
		this->player2 = std::make_shared<Player>();
		this->player2->setClient(client);
		this->player2->setName("Player 2");
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
	if (!this->correctPlayerTurn(player)) {
		player->getClient()->write("U bent nog niet aan de beurt. Wacht totdat de andere speler zijn/haar beurt over is.\r\n");
		return;
	}

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

std::shared_ptr<BaseTurn> Game::getTurn() {
	return this->turn;
}

std::shared_ptr<CharacterCard> Game::getCurrentCalledCharacter() {
	return this->characterOrderDeck->get(currentCharacterCardIndex);
}

#pragma endregion

std::shared_ptr<Deck<std::shared_ptr<CharacterCard>>> Game::getCharacterDeck() {
	return this->characterDeck;
}

std::shared_ptr<Deck<std::shared_ptr<BuildingCard>>> Game::getBuildingDeck() {
	return this->buildingDeck;
}

void Game::printCharacterOrderCard(std::shared_ptr<Player> player) {
	std::string retVal = "";
	for (size_t i = 0; i < this->characterOrderDeck->size(); i++) {

	}
	player->getClient()->write(this->characterOrderDeck->toString());
}


bool Game::isStarted() {
	return started;
}



Game::~Game()
{
}
