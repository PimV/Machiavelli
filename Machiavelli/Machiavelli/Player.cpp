#include "Player.h"


Player::Player()
{
	this->setName("Default");
	this->setAge(18);
	this->setKing(false);
	this->firstToEight = false;
	this->buildingCards = std::vector<std::shared_ptr<BuildingCard>>();
	this->choosableBuildingCards = std::vector<std::shared_ptr<BuildingCard>>();
	this->buildings = std::vector<std::shared_ptr<BuildingCard>>();

	//this->buildings.push_back(std::make_shared<BuildingCard>("lab", Buildings::Laboratorium, 5, Colors::Rood, "lab"));
	//this->buildings.push_back(std::make_shared<BuildingCard>("lab", Buildings::Laboratorium, 5, Colors::Rood, "lab"));
	//this->buildings.push_back(std::make_shared<BuildingCard>("lab", Buildings::Laboratorium, 5, Colors::Rood, "lab"));

	//this->buildings.push_back(std::make_shared<BuildingCard>("lab", Buildings::Laboratorium, 5, Colors::Rood, "lab"));
	//this->buildings.push_back(std::make_shared<BuildingCard>("lab", Buildings::Laboratorium, 5, Colors::Rood, "lab"));
	//this->buildings.push_back(std::make_shared<BuildingCard>("lab", Buildings::Laboratorium, 5, Colors::Rood, "lab"));

	//this->buildings.push_back(std::make_shared<BuildingCard>("lab", Buildings::Laboratorium, 5, Colors::Rood, "lab"));

	this->gold = 0;

	this->client = nullptr;

}

Player::Player(std::string name, int age) {
	this->setName(name);
	this->setAge(age);
	this->setKing(false);
	this->firstToEight = false;
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

std::shared_ptr<CharacterCard> Player::getCharacterCardByCharacter(Characters character) {
	if (character1->getCharacter() == character) {
		return character1;
	}

	if (character2->getCharacter() == character) {
		return character2;
	}
	return nullptr;
}

bool Player::destroyBuilding(std::shared_ptr<Player> destroyer, int index) {
	bool buildingDestroyed = false;
	if (index >= 0 && index < this->buildings.size()) {
		std::shared_ptr<BuildingCard> building = this->buildings.at(index);

		if (building->getName() == "Kerker") {
			destroyer->getClient()->write("U kunt de Kerker niet vernietigen!\r\n");
			return buildingDestroyed;
		}

		if (destroyer->getGold() < building->getDestroyCost()) {
			destroyer->getClient()->write("Kan dit gebouw niet vernietigen; u heeft te weinig goud!\r\n");
		}
		else {
			this->getClient()->write(destroyer->getName() + " heeft het volgende gebouw van u vernietigd: \r\n");
			this->getClient()->write("\t - " + building->toString() + "\r\n");

			destroyer->getClient()->write("U heeft het volgende gebouw van " + this->getName() + " succesvol vernietigd: \r\n");
			destroyer->getClient()->write("\t - " + building->toString() + "\r\n");
			destroyer->getClient()->write("Dit kostte u " + std::to_string(building->getDestroyCost()) + " goudstukken.\r\n");

			destroyer->changeGoldBy(-building->getDestroyCost());
			this->buildings.erase(this->buildings.begin() + index);
			buildingDestroyed = true;
		}

	}
	else {
		this->getClient()->write("Ongeldige index geselecteerd.\r\n");
	}





	return buildingDestroyed;
}

int Player::getConstructedBuildingCount() {
	return this->buildings.size();
}

void Player::setFirstToEight() {
	this->firstToEight = true;
}

bool Player::isFirstToEight() {
	return this->firstToEight;
}

bool Player::checkAllColors() {
	bool hasYellow = false;
	bool hasBlue = false;
	bool hasGreen = false;
	bool hasRed = false;
	bool hasLila = false;

	bool hasSpecialCard = false;

	int correctColors = 0;

	for (size_t i = 0; i < this->buildings.size(); i++) {
		std::shared_ptr<BuildingCard> building = this->buildings.at(i);

		if (building->getName() == "Hof der Wonderen") {
			hasSpecialCard = true;
		}

		switch (building->getColor()) {
		case Colors::Blauw:
			hasBlue = true;
			break;
		case Colors::Geel:
			hasYellow = true;
			break;
		case Colors::Groen:
			hasGreen = true;
			break;
		case Colors::Lila:
			hasLila = true;
			break;
		case Colors::Rood:
			hasRed = true;
			break;
		}
	}

	/* Implement "Hof der Wonderen" card by counting correct colors and adding +1 if needed*/
	if (hasYellow) {
		correctColors++;
	}

	if (hasGreen) {
		correctColors++;
	}
	if (hasBlue) {
		correctColors++;
	}

	if (hasRed) {
		correctColors++;
	}

	if (hasLila) {
		correctColors++;
	}

	if (hasLila && hasSpecialCard) {
		correctColors++;
	}

	if (correctColors >= 5) {
		return true;
	}

	if (hasGreen && hasBlue && hasRed && hasLila && hasYellow) {
		return true;
	}
	return false;
}

int Player::getBuildingPoints() {

	int points = 0;
	for (size_t i = 0; i < this->buildings.size(); i++) {
		//Card specials for "Drakenpoort" and "Universiteit" 
		if (this->buildings.at(i)->getBuilding() == Buildings::Drakenpoort || this->buildings.at(i)->getBuilding() == Buildings::Universiteit) {
			points += 8;
		}
		else {
			points += this->buildings.at(i)->getCost();
		}

	}
	return points;
}

int Player::calculatePoints() {
	int points = 0;

	//Building points
	points += this->getBuildingPoints();

	if (this->checkAllColors()) {
		points += 3;
	}

	if (this->isFirstToEight()) {
		points += 4;
	}

	if (this->getConstructedBuildingCount() >= 8) {
		points += 2;
	}



	return points;
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

bool Player::hasConstructedBuildingByBuilding(Buildings building) {
	for (size_t i = 0; i < this->buildings.size(); i++) {
		if (this->buildings.at(i)->getBuilding() == building) {
			return true;
		}

	}
	return false;
}

std::shared_ptr<BuildingCard> Player::getConstructedBuildingByBuilding(Buildings building) {
	for (size_t i = 0; i < this->buildings.size(); i++) {
		if (this->buildings.at(i)->getBuilding() == building) {
			return this->buildings.at(i);
		}

	}
	return nullptr;
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
			retVal.append("[" + std::to_string(i + 1) + "] - Type: " + card->getName() + ", bouwkosten: " + std::to_string(card->getCost()) + ", kleur: " + card->getColorString() + "\r\n");
		}
	}
	else {
		retVal.append(" - Geen \r\n");
	}

	this->getClient()->write(retVal);
}

std::string Player::printBuildings() {
	std::string retVal = "";
	if (this->buildings.size() > 0) {
		for (std::vector<std::shared_ptr<BuildingCard>>::size_type i = 0; i != this->buildings.size(); i++) {
			std::shared_ptr<BuildingCard> card = this->buildings.at(i);
			retVal.append("[" + std::to_string(i + 1) + "] - Type: " + card->getName() + ", bouwkosten: " + std::to_string(card->getCost()) + ", kleur: " + card->getColorString() + "\r\n");
		}
	}
	else {
		retVal.append(" - Geen \r\n");
	}

	return retVal;
}

std::string Player::printChoosableBuildingCards() {
	std::string retVal = "";
	if (this->choosableBuildingCards.size() > 0) {
		for (std::vector<std::shared_ptr<BuildingCard>>::size_type i = 0; i != this->choosableBuildingCards.size(); i++) {
			std::shared_ptr<BuildingCard> card = this->choosableBuildingCards.at(i);
			retVal.append("[" + std::to_string(i + 1) + "] - Type: " + card->getName() + ", bouwkosten: " + std::to_string(card->getCost()) + ", kleur: " + card->getColorString() + "\r\n");
		}
	}

	return retVal;
}

std::string Player::getBuildingActions() {
	std::string actions = "";
	for (std::vector<std::shared_ptr<BuildingCard>>::size_type i = 0; i != this->buildings.size(); i++) {
		std::shared_ptr<BuildingCard> card = this->buildings.at(i);
		switch (card->getBuilding()) {
		case Buildings::Laboratorium:
			actions.append("\t - laboratorium_special <index> \r\n");
			break;
		case Buildings::Werkplaats:
			actions.append("\t - werkplaats_special \r\n");
			break; 
		}
	}

	return actions;
}

void Player::applyKingEffect() {
	if (this->hasConstructedBuildingByBuilding(Buildings::Magier_School)) {
		this->gold++;
	}

	for (size_t i = 0; i < this->buildings.size(); i++) {
		if (this->buildings.at(i)->getColor() == Colors::Geel) {
			this->gold++;
		}
	}
}

void Player::applyBishopEffect() {
	if (this->hasConstructedBuildingByBuilding(Buildings::Magier_School)) {
		this->gold++;
	}

	for (size_t i = 0; i < this->buildings.size(); i++) {
		if (this->buildings.at(i)->getColor() == Colors::Blauw) {
			this->gold++;
		}
	}
}

void Player::applyMerchantEffect() {
	this->gold++;

	if (this->hasConstructedBuildingByBuilding(Buildings::Magier_School)) {
		this->gold++;
	}

	for (size_t i = 0; i < this->buildings.size(); i++) {
		if (this->buildings.at(i)->getColor() == Colors::Groen) {
			this->gold++;
		}
	}
}

void Player::applyWarlordEffect() {
	if (this->hasConstructedBuildingByBuilding(Buildings::Magier_School)) {
		this->gold++;
	}

	for (size_t i = 0; i < this->buildings.size(); i++) {
		if (this->buildings.at(i)->getColor() == Colors::Rood) {
			this->gold++;
		}
	}
}

void Player::resetBuildingSpecials() {
	for (auto iter : buildings) {
		iter->reset();
	}
}

void Player::resetForNextRound() {
	character1 = nullptr;
	character2 = nullptr;
}

Player::~Player()
{
}
