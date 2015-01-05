#pragma once
#include <iostream>
#include <memory>
#include <vector>

class InputHandler
{
public:
	InputHandler();
	bool handleInput(std::string input);
	std::vector<std::string> splitOnSpace(std::string input);
	virtual ~InputHandler();
private:
	std::shared_ptr<std::vector<std::string>> previousCommands;
};	

