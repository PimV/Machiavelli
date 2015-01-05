#pragma once
#include <iostream>
#include <memory>
#include "Socket.h"

class Player
{
public:
	Player();
	Player(std::string name, int age);

	void setClient(std::shared_ptr<Socket> client);
	std::shared_ptr<Socket> getClient();

	void setName(std::string name);
	std::string getName();

	void setAge(int age);
	int getAge();

	virtual ~Player();
private:
	std::string name;
	int age;
	std::shared_ptr<Socket> client;
};

