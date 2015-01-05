#include "InputHandler.h"



InputHandler::InputHandler()
{
	sp = std::make_shared<Deck<std::shared_ptr<CharacterCard>>>();
	sp2 = std::make_shared<Deck<std::shared_ptr<CharacterCard>>>();

	bc = std::make_shared<Deck<std::shared_ptr<BuildingCard>>>();

	

	std::shared_ptr<CharacterCard> c1 = std::make_shared<CharacterCard>();
	c1->setId(1);
	c1->setName("testcard1");

	//std::shared_ptr<CharacterCard> c2 = std::make_shared<CharacterCard>();
	//c2->setId(2);
	//c2->setName("testcard2");

	//std::shared_ptr<CharacterCard> c3 = std::make_shared<CharacterCard>();
	//c3->setId(3);
	//c3->setName("testcard3");

	
	sp->push(c1);
	//sp->push(c2);
	//sp->push(c3);

	//sp2->push(c1);
	//sp2->push(c2);
	//sp2->push(c3);
}

void InputHandler::handleInput(std::string input) {
	if (input == "print") {

	}
	else if (input == "shuffle") {
		sp->shuffle();
	}
	else if (input == "refill") {

	}
}

InputHandler::~InputHandler()
{
}
