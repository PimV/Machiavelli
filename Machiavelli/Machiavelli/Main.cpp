#include <memory>
#include "Server.h"
#include "Game.h"
#include "InputHandler.h"

int main(int argc, const char * argv[])
{
	std::shared_ptr<Game> game = std::make_shared<Game>();
	std::shared_ptr<InputHandler> inputHandler = std::make_shared<InputHandler>();
	
	inputHandler->setGame(game);
	
	Server::Instance().setInputHandler(inputHandler);
	Server::Instance().setGame(game);
	Server::Instance().run();
	
	return 0;
}

