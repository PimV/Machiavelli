#include <memory>
#include "Server.h"
#include "Game.h"
#include "InputHandler.h"
#include <iostream>


void WINAPI ConsoleHandler(DWORD CEvent)
{
	if (CEvent == CTRL_CLOSE_EVENT)
	{
		//std::cout << "No memory leaks detected.\r\n" << std::endl;
		//std::cout << "Visual Leak Detector is now exiting.\r\n" << std::endl;
		//
		//OutputDebugStringW(L"No memory leaks detected.\r\n");
		//OutputDebugStringW(L"Visual Leak Detector is now exiting.\r\n");
	}
	
}

int main(int argc, const char * argv[])
{
	std::shared_ptr<Game> game = std::make_shared<Game>();
	std::shared_ptr<InputHandler> inputHandler = std::make_shared<InputHandler>();
	
	inputHandler->setGame(game);
	
	Server::Instance().setInputHandler(inputHandler);
	Server::Instance().setGame(game);
	Server::Instance().run();

	std::string input;

	while (input != "quit") {
		std::cin >> input;
	}


	return 0;
}

void start() {

}

