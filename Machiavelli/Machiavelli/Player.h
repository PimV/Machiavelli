#pragma once
#include <iostream>

class Player
{
public:
	Player();
	Player(std::string name, int age);

	void setName(std::string name);
	std::string getName();

	void setAge(int age);
	int getAge();

	virtual ~Player();
private:
	std::string name;
	int age;
};

