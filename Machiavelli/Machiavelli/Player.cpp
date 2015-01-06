#include "Player.h"


Player::Player()
{
	this->setName("Default");
	this->setAge(18);
	this->setKing(false);
	this->setTurn(false);


	this->client = nullptr;

}

Player::Player(std::string name, int age) {
	this->setName(name);
	this->setAge(age);
	this->setKing(false);
	this->setTurn(false);

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

void Player::setTurn(bool turn) {
	this->turn = turn;
}

bool Player::hasTurn() {
	return this->turn;
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


Player::~Player()
{
}
