#pragma once
#include <memory>
#include "Turns.h"
#include "Player.h"

class BaseTurn
{
public:
	BaseTurn();

	void setPlayer(std::shared_ptr<Player> player);
	std::shared_ptr<Player> getPlayer();

	void setType(Turns type);
	Turns getType();

	virtual void setOver(bool done);
	virtual bool isOver();

	virtual std::string printPossibleActions();

	virtual void resetTurn();

	virtual std::string toString();

	virtual ~BaseTurn();
private:
	Turns type;
	std::shared_ptr<Player> player;
	bool done;
};

