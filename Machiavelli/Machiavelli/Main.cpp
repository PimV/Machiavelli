#include <memory>
#include "Server.h"
#include "Game.h"

int main(int argc, const char * argv[])
{
	std::shared_ptr<Game> game = std::make_shared<Game>();
	Server::Instance().setGame(game);
	Server::Instance().run();

	return 0;
}

