#include "InputHandler.h"
#include <string>
#include <sstream>
#include "Server.h"

InputHandler::InputHandler()
{
	previousCommands = std::make_shared<std::vector<std::string>>();
}

bool InputHandler::handleInput(std::string input) {
	if (input.length() > 0) {
		
		std::vector<std::string> params = this->splitOnSpace(input);


		if (params[0] == "broadcast") {
			if (params.size() > 1) {
				Server::Instance().broadcast(params[1]);
			}
		}
		else if (params[0] == "previous_commands") {

		}
		else if (params[0] == "name") {

		}
		else if (params[0] == "disconnect") {
			return false;
		}

		previousCommands->push_back(input);

	}
	return true;
}

std::vector<std::string> InputHandler::splitOnSpace(std::string input) {
	std::stringstream ss(input);
	std::istream_iterator<std::string> begin(ss);
	std::istream_iterator<std::string> end;
	std::vector<std::string> params(begin, end);
	return params;
}

InputHandler::~InputHandler()
{
}
