#include "Server.h"
#include <algorithm>

static Sync_queue<ClientCommand> queue;

Server& Server::Instance() {
	static Server instance;
	return instance;
}

Server::Server()
{
	this->socket_count = 0;
	running = false;
}

void Server::run() {
	running = true;
	// start command consumer thread
	thread consumer(&Server::consume_command, this);
	consumer.detach(); // detaching is usually ugly, but in this case the right thing to do

	//// create a server socket
	ServerSocket server(Server::tcp_port);



	while (running) {	

		try {
			std::cout << "Server awaiting players..." << std::endl;
			Socket* client = nullptr;
			while ((client = server.accept()) != nullptr && running) {
		
				if (this->game->getPlayerCount() >= 2) {
					client->write("Het spijt ons, maar het spel is al begonnen of heeft al genoeg spelers! \r\n");
					client->write("Probeer het later nog eens! \r\n");
					break;
				}
				//Create new socket
				thread handler{ &Server::handle_client, this, client };
				handler.detach();	

				std::cout << "Server awaiting players... again..." << std::endl;

				//Increase socket/player count
				this->socket_count++;
				cerr << "Players: " << this->socket_count << std::endl;
			}
		}
		catch (const exception& ex) {
			cerr << ex.what() << ", resuming..." << '\n';
		}
	}

}

void Server::stop() {
	OutputDebugStringW(L"Stopping server...\r\n");
	running = false;
}


void Server::setGame(std::shared_ptr<Game> game) {
	this->game = game;
}

void Server::setInputHandler(std::shared_ptr<InputHandler> inputHandler) {
	this->inputHandler = inputHandler;
}

void Server::handle_client(Socket* socket)
{
	shared_ptr<Socket> client{ socket };

	client->write("Welkom bij het spel 'Machiavelli', gemaakt door James Hay en Pim Verlangen.\r\n");
	client->write("U kunt uw naam op ieder willekeurig moment veranderen door 'naam <name>' in te vullen.\r\n");
	client->write(Server::prompt);

	this->clients.push_back(client);
	this->game->addPlayer(client);



	while (running) { // game loop
		try {
			// read first line of request
			string cmd = client->readline();

			//Last 'if' is a "Failsafe" for client incorrectly closing connection
			if (cmd == "quit" || cmd == "disconnect" || cmd == "freakinguglywayofsayingyouhaveclosedtheclientbypressingexit") {
				this->clients.erase(std::remove(this->clients.begin(), this->clients.end(), client), this->clients.end());
				this->game->removePlayer(client);
				this->socket_count--;
				break; // out of game loop, will end this thread and close connection
			}

			ClientCommand command{ cmd, client };
			queue.put(command);

		}
		catch (const exception& ex) {

			client->write("ERROR: ");
			client->write(ex.what());
			client->write("\n");
		}
		catch (...) {
			client->write("ERROR: something went wrong during handling of your request. Sorry!\n");
		}
	}
}

void Server::broadcast(std::string msg) {
	std::cout << "<Broadcast>: " << msg << std::endl;
	for (std::vector<std::shared_ptr<Socket>>::size_type i = 0; i != clients.size(); i++) {
		clients[i]->write(msg);
		clients[i]->write("\r\n");
	}
}

void Server::consume_command() // runs in its own thread
{
	while (running) {
		ClientCommand command;
		queue.get(command); // will block here unless there still are command objects in the queue
		shared_ptr<Socket> client{ command.get_client() };
		if (client) {
			try {
				std::shared_ptr<Player> playerTalking = this->game->getPlayerByClient(client);

				this->inputHandler->handleInput(command.get_cmd(), playerTalking);
			}
			catch (const exception& ex) {
				client->write("Sorry, ");
				client->write(ex.what());
				client->write("\n");
			}
			catch (...) {
				client->write("Sorry, something went wrong during handling of your request.\n");
			}
			client->write(Server::prompt);
		}
		else {
			cerr << "trying to handle command for client who has disappeared...\n";
		}
	}
}


Server::~Server()
{
}
