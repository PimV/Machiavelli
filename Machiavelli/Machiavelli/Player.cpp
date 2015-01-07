#include "Player.h"


Player::Player()
{
	this->setName("Default");
	this->setAge(18);
	this->setKing(false);
	this->setTurn(false);
	this->buildingCards = std::vector<std::shared_ptr<BuildingCard>>();
	this->choosableBuildingCards = std::vector<std::shared_ptr<BuildingCard>>();
	this->buildings = std::vector<std::shared_ptr<BuildingCard>>();

	this->gold = 0;

	this->client = nullptr;

}

Player::Player(std::string name, int age) {
	this->setName(name);
	this->setAge(age);
	this->setKing(false);
	this->setTurn(false);
	this->buildingCards = std::vector<std::shared_ptr<BuildingCard>>();
	this->choosableBuildingCards = std::vector<std::shared_ptr<BuildingCard>>();
	this->buildings = std::vector<std::shared_ptr<BuildingCard>>();

	this->gold = 0;

}

void Player::setName(std::string name) {
	this->name = name;
}

std::string Player::getName() {
	return this->name;
}

void Player::setClient(std::shared_ptr<Socket> client) {
	this->client = client;
}

std::shared_ptr<Socket> Player::getClient() {
	return this->client;
}

void Player::setAge(int age) {
	this->age = age;
}

int Player::getAge() {
	return this->age;
}

void Player::setKing(bool isKing) {
	this->isKing = isKing;
}

bool Player::playerIsKing() {
	return this->isKing;
}


void Player::changeGoldBy(int amount) {
	this->gold += amount;
	if (this->gold < 0) {
		this->gold = 0;
	}
}

int Player::getGold() {
	return this->gold;
}

void Player::setActiveCharacterCard(std::shared_ptr<CharacterCard> card) {
	if (character1->getCharacter() == card->getCharacter()) {
		this->activeCharacter = character1;
	}
	else if (character2->getCharacter() == card->getCharacter()) {
		this->activeCharacter = character2;
	}

}

bool Player::constructBuildingCard(int index) {
	bool buildingConstructed = false;
	if (index >= 0 && index < this->buildingCards.size()) {
		std::shared_ptr<BuildingCard> card = this->buildingCards.at(index);

		if (this->getGold() >= card->getCost()) {
			this->buildings.push_back(card);
			this->buildingCards.erase(this->buildingCards.begin() + index);
			buildingConstructed = true;
			this->changeGoldBy(-card->getCost());
			this->getClient()->write("U heeft het gebouw " + card->getName() + " gebouwd voor " + std::to_string(card->getCost()) + " goudstukken.\r\n");
			this->getClient()->write("U heeft nog " + std::to_string(this->getGold()) + " goudstukken over.\r\n");
		}
		else {
			this->getClient()->write("U heeft " + std::to_string(card->getCost()) + " goudstukken nodig en u heeft er maar " + std::to_string(this->getGold()) + ".\r\n");
		}
	}
	else {
		this->getClient()->write("Ongeldige index geselecteerd.\r\n");
	}

	return buildingConstructed;
}

std::shared_ptr<CharacterCard> Player::getActiveCharacterCard() {
	return this->activeCharacter;
}

bool Player::takeCharacterCard(std::shared_ptr<CharacterCard> card) {
	if (this->character1 == nullptr) {
		character1 = card;
		return true;
	}

	if (this->character2 == nullptr) {
		character2 = card;
		return true;
	}

	this->getClient()->write("U heeft al twee karakterkaarten, namelijk: [" + character1->getCharacterString() + "] en [" + character2->getCharacterString() + "]\r\n");
	return false;
}

bool Player::hasCharacterChard(std::shared_ptr<CharacterCard> card) {
	if (this->character1->getCharacter() == card->getCharacter() || this->character2->getCharacter() == card->getCharacter()) {
		return true;
	}

	return false;
}

bool Player::hasCharacterCardByCharacter(Characters character) {
	if ((character1 != nullptr && character1->getCharacter() == character) || (character2 != nullptr && character2->getCharacter() == character)) {
		return true;
	}
	return false;
}

void Player::addBuildingCard(std::shared_ptr<BuildingCard> card) {
	this->buildingCards.push_back(card);
}

void Player::addChoosableBuildingCards(std::shared_ptr<BuildingCard> card) {
	this->choosableBuildingCards.push_back(card);
}

std::shared_ptr<BuildingCard> Player::getChoosableBuildingCardByIndex(int index) {
	if (index >= 0 && index < this->choosableBuildingCards.size()) {
		return this->choosableBuildingCards.at(index);
	}
	return nullptr;
}

void Player::emptyChoosableBuildingCards() {
	this->choosableBuildingCards.clear();
}

void Player::removeBuildingCard(std::shared_ptr<BuildingCard> card) {
	for (std::vector<std::shared_ptr<BuildingCard>>::size_type i = 0; i != this->buildingCards.size(); i++) {
		if (this->buildingCards.at(i) == card) {
			this->buildingCards.erase(this->buildingCards.begin() + i);
			break;
		}
	}
}



std::vector<std::shared_ptr<BuildingCard>> Player::getBuildingCards() {
	return this->buildingCards;
}

void Player::setBuildingCards(std::vector<std::shared_ptr<BuildingCard>> cards) {
	this->buildingCards = cards;
}

void  Player::printBuildingCards() {
	std::string retVal = "Uw bouwkaarten: \r\n";
	if (this->buildingCards.size() > 0) {
		for (std::vector<std::shared_ptr<BuildingCard>>::size_type i = 0; i != this->buildingCards.size(); i++) {
			std::shared_ptr<BuildingCard> card = this->buildingCards.at(i);
			retVal.append("[" + std::to_string(i+1) +  "] - Type: " + card->getName() + ", bouwkosten: " + std::to_string(card->getCost()) + ", kleur: " + card->getColorString() + "\r\n");
		}
	}
	else {
		retVal.append(" - Geen \r\n");
	}

	this->getClient()->write(retVal);
}

void Player::printChoosableBuildingCards() {
	std::string retVal = "Pak één van de twee kaarten (de andere wordt gedekt op tafel gelegd): \r\n";
	if (this->choosableBuildingCards.size() > 0) {
		for (std::vector<std::shared_ptr<BuildingCard>>::size_type i = 0; i != this->choosableBuildingCards.size(); i++) {
			std::shared_ptr<BuildingCard> card = this->choosableBuildingCards.at(i);
			retVal.append("[" + std::to_string(i + 1) + "] - Type: " + card->getName() + ", bouwkosten: " + std::to_string(card->getCost()) + ", kleur: " + card->getColorString() + "\r\n");
		}
	}

	this->getClient()->write(retVal);
}

void Player::resetForNextRound() {
	character1 = nullptr;
	character2 = nullptr;
}

Player::~Player()
{
}
