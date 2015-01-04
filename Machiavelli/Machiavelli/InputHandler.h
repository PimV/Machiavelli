#pragma once
#include <iostream>

class InputHandler
{
public:
	InputHandler();
	void handleInput(std::string input);
	virtual ~InputHandler();
};

