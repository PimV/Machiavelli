#pragma once
#include <iostream>
#include <memory>

#include "Deck.h"
#include "CharacterCard.h"
#include "BuildingCard.h"

class InputHandler
{
public:
	InputHandler();
	void handleInput(std::string input);
	virtual ~InputHandler();
private:
	std::shared_ptr<Deck<std::shared_ptr<CharacterCard>>> sp;
	std::shared_ptr<Deck<std::shared_ptr<CharacterCard>>> sp2;
	std::shared_ptr<Deck<std::shared_ptr<BuildingCard>>> bc;
};

