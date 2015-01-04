#include "Player.h"


Player::Player()
{
	this->name = "Default";
	this->age = 18;
}

Player::Player(std::string name, int age) {
	this->setName(name);
	this->setAge(age);
}

void Player::setName(std::string name) {
	this->name = name;
}

std::string Player::getName() {
	return this->name;
}

void Player::setAge(int age) {
	this->age = age;
}

int Player::getAge() {
	return this->age;
}


Player::~Player()
{
}
