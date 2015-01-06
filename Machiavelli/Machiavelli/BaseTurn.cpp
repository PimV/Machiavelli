#include "BaseTurn.h"


BaseTurn::BaseTurn()
{
	this->type = Turns::UNDEFINED;
	this->done = false;
}

void BaseTurn::setPlayer(std::shared_ptr<Player> player) {
	this->player = player;
}

std::shared_ptr<Player> BaseTurn::getPlayer() {
	return this->player;
}

Turns BaseTurn::getType() {
	return this->type;
}

void BaseTurn::setType(Turns type) {
	this->type = type;
}

void BaseTurn::setOver(bool done) {
	this->done = done;
}

bool BaseTurn::isOver() {
	return true;
}

void BaseTurn::resetTurn() {
	this->done = false;
}

BaseTurn::~BaseTurn()
{
}
