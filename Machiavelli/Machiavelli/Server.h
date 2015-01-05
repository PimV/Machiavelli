#pragma once
#include <thread>
#include <iostream>
#include <exception>
#include <memory>
using namespace std;

#include "InputHandler.h"

#include "Socket.h"
#include "Sync_queue.h"
#include "ClientCommand.h"
#include "Game.h"

class Server
{
public:
	Server();

	static Server& Instance();

	void run();
	void handle_client(Socket* socket, std::shared_ptr<InputHandler> ih);
	void consume_command();
	void broadcast(std::string msg);

	int getSocketCount();

	void setGame(std::shared_ptr<Game> game);
	std::shared_ptr<Game> getGame();

	virtual ~Server();

private:
	std::vector<std::shared_ptr<Socket>> clients;
	std::shared_ptr<Game> game;
	int socket_count;
	const int tcp_port { 1080 };
	const std::string prompt { "> " };

};
