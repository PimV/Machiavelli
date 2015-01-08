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

void BaseTurn::resetTurn() { }

std::string BaseTurn::printPossibleActions() { return "actions";  }

std::string BaseTurn::toString() { return "baseturn"; }

BaseTurn::~BaseTurn()
{
}
