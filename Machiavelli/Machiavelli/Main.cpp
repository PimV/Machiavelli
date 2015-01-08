#include <memory>
#include <vld.h>
#include "Server.h"
#include "Game.h"
#include "InputHandler.h"
#include <iostream>


void WINAPI ConsoleHandler(DWORD CEvent)
{
	if (CEvent == CTRL_CLOSE_EVENT)
	{
		OutputDebugStringW(L"Stop!\r\n");
		//Server::Instance().stop();
	}
	
}

int main(int argc, const char * argv[])
{
	if (SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler, TRUE) == FALSE)
	{
		
	}
	std::shared_ptr<Game> game = std::make_shared<Game>();
	std::shared_ptr<InputHandler> inputHandler = std::make_shared<InputHandler>();

	inputHandler->setGame(game);

	Server::Instance().setInputHandler(inputHandler);
	Server::Instance().setGame(game);
	Server::Instance().run();

	std::getchar();
	

	return 0;
}

