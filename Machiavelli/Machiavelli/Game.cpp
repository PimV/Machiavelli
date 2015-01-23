#include "Game.h"
#include "Server.h"


Game::Game()
{
	this->loader = std::make_unique<Loader>();
	rounds = 0;
	turns = 0;

	this->characterOrderDeck = loader->loadCharacterCards(std::make_shared<Deck<std::shared_ptr<CharacterCard>>>());

	this->started = false;
	this->lastRound = false;
	this->lastTurn = false;
	this->gameOver = false;
}

void Game::startGame() {
	//Set first player to king
	this->player1->setKing(true);

	//Build and shuffle buildingDeck
	this->buildingDeck = loader->loadBuildingCards(std::make_shared<Deck<std::shared_ptr<BuildingCard>>>());
	this->buildingDeck->shuffle();

	//Officially start game (input flag)
	this->gameOver = false;
	this->lastRound = false;
	this->lastTurn = false;
	this->startRound();
	this->started = true;

}

void Game::endGame() {
	Server::Instance().broadcast("De uitkomst van het spel is: ");
	Server::Instance().broadcast("Speler 1 (" + player1->getName() + "): " + std::to_string(player1->calculatePoints()));
	Server::Instance().broadcast("Speler 2 (" + player2->getName() + "): " + std::to_string(player2->calculatePoints()));
	if (player1->calculatePoints() > player2->calculatePoints()) {
		Server::Instance().broadcast(player1->getName() + " heeft gewonnen!");
	}
	else if (player2->calculatePoints() > player1->calculatePoints()) {
		Server::Instance().broadcast(player2->getName() + " heeft gewonnen!");
	}
	else if (player2->calculatePoints() == player1->calculatePoints()) {
		if (player1->getBuildingPoints() > player2->getBuildingPoints()) {
			Server::Instance().broadcast(player1->getName() + " heeft gewonnen doordat hij/zij meer punten had met uitsluitend gebouwen!");
		}
		else if (player2->getBuildingPoints() > player1->getBuildingPoints()) {
			Server::Instance().broadcast(player2->getName() + " heeft gewonnen doordat hij/zij meer punten had met uitsluitend gebouwen!");
		}
	}
	Server::Instance().broadcast("Het spel is nu voorbij. Bedankt voor het spelen!");
	this->gameOver = true;
}

bool Game::isOver() {
	return this->gameOver;
}

void Game::startRound() {
	if (this->lastRound) {
		this->endGame();
		return;
	}

	Server::Instance().broadcast("Ronde: " + std::to_string(rounds + 1));

	//Load initial decks
	this->characterDeck = loader->loadCharacterCards(std::make_shared<Deck<std::shared_ptr<CharacterCard>>>());
	this->characterDeck->shuffle();
	Server::Instance().broadcast("De karakterkaarten zijn geschud voor de nieuwe ronde.");

	for (size_t i = 0; i < this->characterOrderDeck->size(); i++) {
		this->characterOrderDeck->get(i)->setMurdered(false);
		this->characterOrderDeck->get(i)->setPickpocketed(false);
	}

	std::shared_ptr<CharacterSelectionTurn> startingTurn = std::make_shared<CharacterSelectionTurn>();
	this->turn = startingTurn;

	//Check for King Card
	if (this->player1->hasCharacterCardByCharacter(Characters::Koning)) {
		Server::Instance().broadcast("Omdat " + player1->getName() + " de afgelopen ronde de Koning had, is hij deze ronde de koning.\r\n");
		Server::Instance().broadcast(player1->getName() + " mag daarom beginnen.");
		this->player1->setKing(true);
		this->player2->setKing(false);

	}
	else if (this->player2->hasCharacterCardByCharacter(Characters::Koning)) {
		Server::Instance().broadcast("Omdat " + player2->getName() + " de afgelopen ronde de Koning had, is hij deze ronde de koning.");
		Server::Instance().broadcast(player2->getName() + " mag daarom beginnen.");
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

	//Server::Instance().broadcast(this->turn->getPlayer()->getName() + " is aan de beurt.");

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
		else {
			this->showCharacterDeckOptions(player);
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
	std::cout << "Current character order: " << currentCharacterCardIndex << std::endl;
	for (size_t i = currentCharacterCardIndex; i < this->characterOrderDeck->size(); i++) {
		currentCharacterCardIndex = i + 1;
		std::shared_ptr<CharacterCard> card = this->characterOrderDeck->get(i);
		Server::Instance().broadcast("De Koning roept het karakter \"" + card->getCharacterString() + "\" op.");
		bool pickpocketed = card->isPickpocketed();
		bool murdered = card->isMurdered();

		if (murdered) {
			if (player1->hasCharacterCardByCharacter(card->getCharacter()) || player2->hasCharacterCardByCharacter(card->getCharacter())) {
				turns++;
				this->turn->setOver(true);
			}
			//currentCharacterCardIndex = i + 2;

			std::cout << "Skipping turn cause " << card->getCharacterString() << " was murdered. char index:" << currentCharacterCardIndex << std::endl;
			continue;
		}

		std::shared_ptr<Player> player = nullptr;
		if (player1->hasCharacterChard(card)) {
			card = player1->getCharacterCardByCharacter(card->getCharacter());
			player = player1;
		}
		else if (player2->hasCharacterChard(card)) {
			card = player2->getCharacterCardByCharacter(card->getCharacter());
			player = player2;
		}




		if (player1->hasCharacterChard(card)) {
			if (pickpocketed) {
				Server::Instance().broadcast("Omdat " + player2->getName() + " de Dief achter de " + card->getCharacterString() + " heeft gestuurd en " + player1->getName()
					+ " dit karakter gekozen heeft, krijgt " + player2->getName() + " al het goud van " + player1->getName() + " (goudstukken: " + std::to_string(player1->getGold()));
				//Server::Instance().broadcast(player2->getName() + " heeft de Dief achter een karakter van " + player1->getName() + " gestuurd.");
				//Server::Instance().broadcast(player2->getName() + " krijgt daarom al het goud van " + player1->getName() + "(" + std::to_string(player1->getGold()) + ")");
				player2->changeGoldBy(player1->getGold());
				player1->changeGoldBy(-player1->getGold());
			}
			this->turn->setPlayer(player1);
			player1->setActiveCharacterCard(card);
			//i = currentCharacterCardIndex;
			currentCharacterCardIndex = i + 1;

			break;
		}
		else if (player2->hasCharacterChard(card)) {
			if (pickpocketed) {
				Server::Instance().broadcast("Omdat " + player1->getName() + " de Dief achter de " + card->getCharacterString() + " heeft gestuurd en " + player2->getName()
					+ " dit karakter gekozen heeft, krijgt " + player1->getName() + " al het goud van " + player2->getName() + " (goudstukken: " + std::to_string(player2->getGold()));
				player1->changeGoldBy(player2->getGold());
				player2->changeGoldBy(-player2->getGold());
			}
			this->turn->setPlayer(player2);
			player2->setActiveCharacterCard(card);
			//i = currentCharacterCardIndex;
			currentCharacterCardIndex = i + 1;

			break;
		}
	}

	if (this->turn->isOver()) {
		switchTurn();
	}
	else {
		Server::Instance().broadcast(this->turn->getPlayer()->getName() + " is aan de beurt met de " + this->turn->getPlayer()->getActiveCharacterCard()->getCharacterString());

		this->applyCardEffects(this->turn->getPlayer());
		this->turn->getPlayer()->getClient()->write(this->turn->printPossibleActions());
	}

	


	//this->turn->getPlayer()->getClient()->write(this->turn->getPlayer()->getActiveCharacterCard()->possibleActions());
}

void Game::printPossibleActions(std::shared_ptr<Player> player) {
	if (!this->correctPlayerTurn(player)) {
		player->getClient()->write("U bent nog niet aan de beurt. Wacht totdat de andere speler zijn/haar beurt over is.\r\n");
		return;
	}

	this->turn->getPlayer()->getClient()->write(this->turn->printPossibleActions());
}

void Game::workshopSpecial(std::shared_ptr<Player> player) {
	if (this->turn->getType() != Turns::CHAR_ACTION) {
		player->getClient()->write("Deze actie kan nog niet uitgevoerd worden, omdat het spel in een andere fase zit.\r\n");
		return;
	}

	if (!this->correctPlayerTurn(player)) {
		player->getClient()->write("U bent nog niet aan de beurt. Wacht totdat de andere speler zijn/haar beurt over is.\r\n");
		return;
	}

	if (!this->correctBuildingTurn(player, Buildings::Werkplaats)) {
		player->getClient()->write("U kunt de speciale actie van de Werkplaats niet gebruiken, omdat u dit gebouw niet heeft gebouwd.\r\n");
		return;
	}
	std::shared_ptr<BuildingCard> building = player->getConstructedBuildingByBuilding(Buildings::Werkplaats);

	if (building->didSpecial()) {
		player->getClient()->write("U heeft deze speciale actie van de Werkplaats deze beurt al eens uitgevoerd.\r\n");
		return;
	}

	if (player->getGold() >= 3) {
		Server::Instance().broadcast(player->getName() + " betaalt 3 goudstukken om twee nieuwe gebouwkaarten te pakken.");
		player->getClient()->write("U heeft de volgende 2 bouwkaarten gepakt: \r\n");

		std::shared_ptr<BuildingCard> newCard = this->buildingDeck->pop();
		player->getClient()->write("\t " + newCard->toString() + "\r\n");
		player->addBuildingCard(newCard);

		newCard = this->buildingDeck->pop();
		player->getClient()->write("\t " + newCard->toString() + "\r\n");
		player->addBuildingCard(newCard);
		player->changeGoldBy(-3);

		building->doSpecial();
	}
	else {
		player->getClient()->write("U heeft niet genoeg goud om deze speciale actie uit te voeren (minimaal 3 goud nodig).\r\n");
	}
}

void Game::labSpecial(std::shared_ptr<Player> player, int index) {
	if (this->turn->getType() != Turns::CHAR_ACTION) {
		player->getClient()->write("Deze actie kan nog niet uitgevoerd worden, omdat het spel in een andere fase zit.\r\n");
		return;
	}

	if (!this->correctPlayerTurn(player)) {
		player->getClient()->write("U bent nog niet aan de beurt. Wacht totdat de andere speler zijn/haar beurt over is.\r\n");
		return;
	}

	if (!this->correctBuildingTurn(player, Buildings::Laboratorium)) {
		player->getClient()->write("U kunt de speciale actie van het Laboratorium niet gebruiken, omdat u dit gebouw niet heeft gebouwd.\r\n");
		return;
	}
	std::shared_ptr<BuildingCard> building = player->getConstructedBuildingByBuilding(Buildings::Laboratorium);

	if (building->didSpecial()) {
		player->getClient()->write("U heeft deze speciale actie van het Laboratorium deze beurt al eens uitgevoerd.\r\n");
		return;
	}

	if (player->getBuildingCards().size() > 0) {
		if (index >= 0 && index < player->getBuildingCards().size()) {
			Server::Instance().broadcast(player->getName() + " krijgt 1 goudstuk voor het afleggen van een bouwkaart.");
			std::shared_ptr<BuildingCard> card = player->getBuildingCards().at(index);
			player->getClient()->write("U heeft de volgende bouwkaart afgelegd: \r\n");
			player->getClient()->write("\t" + card->toString() + "\r\n");
			player->changeGoldBy(1);
			player->removeBuildingCard(card);
			building->doSpecial();
		}
		else {
			player->getClient()->write("Ongeldige index geselecteerd.\r\n");
		}
	}
	else {
		player->getClient()->write("U heeft niet genoeg bouwkaarten om deze actie uit te voeren.\r\n");
	}
}

void Game::applyCardEffects(std::shared_ptr<Player> player) {
	std::shared_ptr<CharacterActionTurn> caTurn = std::dynamic_pointer_cast<CharacterActionTurn>(this->turn);
	switch (player->getActiveCharacterCard()->getCharacter()) {
	case Characters::Koning:
		player->applyKingEffect();
		break;
	case Characters::Prediker:
		player->applyBishopEffect();
		break;
	case Characters::Koopman:
		player->applyMerchantEffect();
		break;
	case Characters::Bouwmeester:
		caTurn->setMaxBuildings(3);
		for (int i = 0; i < 2; i++) {
			player->addBuildingCard(this->buildingDeck->pop());
		}
		break;
	case Characters::Condottiere:
		player->applyWarlordEffect();
		break;
	}
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

	if (player->hasConstructedBuildingByBuilding(Buildings::Bibliotheek)) {
		player->getClient()->write("Omdat u de Bibliotheek heeft gebouwd, mag u beide getrokken kaarten direct in uw hand nemen: \r\n");
		player->getClient()->write(player->printChoosableBuildingCards());

		//Two possibilities
		player->addBuildingCard(player->getChoosableBuildingCardByIndex(0));
		player->addBuildingCard(player->getChoosableBuildingCardByIndex(1));

		player->emptyChoosableBuildingCards();
		caTurn->selectSingleCardFromDeck();

	}
	else {
		player->getClient()->write("Pak één van de twee kaarten (de andere wordt gedekt op tafel gelegd) via \"selecteer_bouwkaart <index>\": \r\n");
		player->getClient()->write(player->printChoosableBuildingCards());
	}



	caTurn->takeCardsFromDeck();
}

void Game::removeBuilding(std::shared_ptr<Player> player, int index) {
	std::shared_ptr<CharacterActionTurn> caTurn = std::dynamic_pointer_cast<CharacterActionTurn>(turn);
	if (caTurn->didSpecial()) {
		player->getClient()->write("U heeft al uw speciale karaktereigenschap gebruikt deze beurt!\r\n");
		return;
	}

	std::shared_ptr<Player> opponent = nullptr;
	if (player == player1) {
		opponent = player2;
	}
	else if (player == player2) {
		opponent = player1;
	}

	if (opponent->hasCharacterCardByCharacter(Characters::Prediker)) {
		player->getClient()->write("Uw tegenstander heeft een kaart met het karakter Prediker, waardoor u zijn/haar gebouwen deze ronde niet kan vernietigen!\r\n");
		return;
	}

	if (opponent->getConstructedBuildingCount() >= 8) {
		player->getClient()->write("Uw tegenstander heeft al 8 of meer gebouwen. U kunt daarom geen gebouwen vernietigen van uw tegenstander!\r\n");
		return;
	}

	bool success = opponent->destroyBuilding(player, index);
	if (success) {
		caTurn->doSpecial();
	}
}

void Game::printRemoveOptions(std::shared_ptr<Player> player) {
	std::shared_ptr<CharacterActionTurn> caTurn = std::dynamic_pointer_cast<CharacterActionTurn>(turn);
	if (caTurn->didSpecial()) {
		player->getClient()->write("U heeft al uw speciale karaktereigenschap gebruikt deze beurt!\r\n");
		return;
	}

	std::shared_ptr<Player> opponent = nullptr;
	if (player == player1) {
		opponent = player2;
	}
	else if (player == player2) {
		opponent = player1;
	}

	player->getClient()->write("De gebouwen van uw tegenstander: \r\n");
	player->getClient()->write(opponent->printBuildings());

}

void Game::printOpponentBuildings(std::shared_ptr<Player> player) {
	std::shared_ptr<Player> opponent = nullptr;
	if (player == player1) {
		opponent = player2;
	}
	else if (player == player2) {
		opponent = player1;
	}

	player->getClient()->write("De gebouwen van uw tegenstander: \r\n");
	player->getClient()->write(opponent->printBuildings());
}

void Game::constructBuilding(std::shared_ptr<Player> player, int index) {
	std::shared_ptr<CharacterActionTurn> caTurn = std::dynamic_pointer_cast<CharacterActionTurn>(turn);
	if (caTurn->buildingsBuilt()) {
		player->getClient()->write("U heeft in deze beurt al het maximaal aantal gebouwen gebouwd (" + std::to_string(caTurn->getMaxBuildings()) + ").\r\n");
		return;
	}
	bool success = player->constructBuildingCard(index);
	if (success){
		caTurn->buildBuilding();
	}


	if (player->getConstructedBuildingCount() >= 8) {
		if (player == player1) {
			if (!player2->isFirstToEight()) {
				player1->setFirstToEight();
				this->lastRound = true;
				std::cout << "Player 1 is first to eight buildings, last round initiated" << std::endl;
			}
		}
		else if (player == player2) {
			if (!player1->isFirstToEight()) {
				player2->setFirstToEight();
				this->lastRound = true;
				std::cout << "Player 2 is first to eight buildings, last round initiated" << std::endl;
			}
		}
	}
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
		if (card->getCharacter() == Characters::Moordenaar) {
			player->getClient()->write("U kunt niet stelen van de Moordenaar!\r\n");
			return;
		}

		if (card->isMurdered()) {
			player->getClient()->write("U kunt niet van een vermoord karakter stelen!\r\n");
			return;
		}

		card->setPickpocketed(true);
		Server::Instance().broadcast(player->getName() + " heeft de Dief achter de " + card->getCharacterString() + " gestuurd.\r\n");
		//player->getClient()->write("U heeft de Dief achter de " + card->getCharacterString() + " gestuurd.\r\n");

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

bool Game::correctBuildingTurn(std::shared_ptr<Player> player, Buildings building) {
	if (player->hasConstructedBuildingByBuilding(building)) {
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
		player->getClient()->write("[" + std::to_string(i + 1) + "] " + this->characterDeck->get(i)->getCharacterString() + " (Roepkaart: " + std::to_string(this->characterDeck->get(i)->getId()) + ")\r\n");
	}
}

bool Game::checkTurnSwitch() {
	if (this->turn->isOver()) {
		return true;
	}
	return false;
}

void Game::switchTurn() {
	if (currentCharacterCardIndex == 7 && lastRound) {
		this->lastTurn = true;
	}

	if (this->turn->isOver()) {
		turns++;
		if (this->turn->getPlayer() == player1) {
			if (this->turn->getType() == Turns::CHAR_SELECT) {
				Server::Instance().broadcast(player2->getName() + " is nu aan de beurt.");
			}

			this->turn->setPlayer(player2);
		}
		else if (this->turn->getPlayer() == player2) {
			if (this->turn->getType() == Turns::CHAR_SELECT) {
				Server::Instance().broadcast(player1->getName() + " is nu aan de beurt.");
			}

			this->turn->setPlayer(player1);
			std::cout << "Turn to player1" << std::endl;
		}

		//Reset building specials
		player1->resetBuildingSpecials();
		player2->resetBuildingSpecials();

		//Switch turn types
		this->switchTurnTypes();

		//Reset current turn;
		this->turn->resetTurn();


		//Prepare clients for new turn
		this->prepareTurn();

		std::cout << "Server switching turns..." << std::endl;
	}
}

bool Game::switchTurnTypes() {
	if (turns % 4 == 0) {
		if (this->turn->getType() == Turns::CHAR_SELECT) {
			//Switch to character action turn
			this->turn = std::make_shared<CharacterActionTurn>();
			std::cout << "turn type switch to action" << std::endl;
			return true;
		}
		else if (this->turn->getType() == Turns::CHAR_ACTION) {
			std::cout << "switching turns plzzz" << std::endl;
			this->startRound();
			player1->resetForNextRound();
			player2->resetForNextRound();
			return true;
		}
	}
	return false;
}

void Game::prepareTurn() {
	if (this->turn->getType() == Turns::CHAR_SELECT) {
		std::cout << "Showing character deck options to player" << std::endl;
		//this->turn->getPlayer()->getClient()->write("U bent aan de beurt en moet nu één kaart bedekken (dek <kaart_index>) en één kaart pakken (pak <kaart_index>).\r\n");
		this->showCharacterDeckOptions(this->turn->getPlayer());
	}
	else if (this->turn->getType() == Turns::CHAR_ACTION) {
		if (!gameOver){
			std::cout << "Asking player what to do with this character" << std::endl;
			this->callCharacterCard();
		}



	}
}

void Game::printOnTurn(std::shared_ptr<Player> player) {
	player->getClient()->write(this->turn->getPlayer()->getName() + " is aan de beurt.\r\n");
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
