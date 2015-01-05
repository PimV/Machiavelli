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
}

void Server::run() {
	// start command consumer thread
	thread consumer(&Server::consume_command, this);
	consumer.detach(); // detaching is usually ugly, but in this case the right thing to do

	//// create a server socket
	ServerSocket server(Server::tcp_port);
	auto ih = make_shared<InputHandler>();

	while (true) {
		try {
			std::cout << "Server awaiting players..." << std::endl;
			Socket* client = nullptr;

			while ((client = server.accept()) != nullptr) {
				if (this->game->getPlayerCount() >= 2) {
					client->write("Sorry, game is full/has started!");
					break;
				}
				//Create new socket
				thread handler{ &Server::handle_client, this, client, ih };
				handler.detach();

				//Add sockets to socket collection
				std::shared_ptr<Socket> shared_client_ptr(client);
				this->clients.push_back(shared_client_ptr);
				this->game->addPlayer(shared_client_ptr);

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

void Server::setGame(std::shared_ptr<Game> game) {
	this->game = game;
}

void Server::handle_client(Socket* socket, std::shared_ptr<InputHandler> ih) 
{
	shared_ptr<Socket> client{ socket };
	
	client->write("Welkom bij het spel 'Machiavelli', gemaakt door James Hay en Pim Verlangen.\r\n");
	client->write("U kunt uw naam op ieder willekeurig moment veranderen door 'name <name>' in te vullen.\r\n");

	while (true) { // game loop
		try {
			// read first line of request
			string cmd = client->readline();

			if (ih->handleInput(cmd) == false) {
				this->game->removePlayer(client);
				this->socket_count--; 
				break;
			}
	

			//if (cmd == "quit") {
			//	this->game->removePlayer(client);
			//	this->socket_count--;
			//	client->write("Bye!\n");
			//	break; // out of game loop, will end this thread and close connection
			//}

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
	std::cout << "Server sending global message: " << std::endl;
	std::cout << msg << std::endl;
	for (std::vector<std::shared_ptr<Socket>>::size_type i = 0; i != clients.size(); i++) {
		clients[i]->write(msg);
		clients[i]->write("\n");
	}
	std::cout << "Global message sent." << std::endl;
}

void Server::consume_command() // runs in its own thread
{
	while (true) {
		ClientCommand command;
		queue.get(command); // will block here unless there still are command objects in the queue
		shared_ptr<Socket> client{ command.get_client() };
		if (client) {
			try {
				// TODO handle command here
				client->write("Hey, you wrote: '");
				client->write(command.get_cmd());
				client->write("', but I'm not doing anything with it.\n");
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
