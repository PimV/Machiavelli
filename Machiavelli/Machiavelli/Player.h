#pragma once
#include <iostream>
#include <memory>
#include "Socket.h"
#include "CharacterCard.h"

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

	void setKing(bool isKing);
	bool playerIsKing();

	void setTurn(bool turn);
	bool hasTurn();

	bool takeCharacterCard(std::shared_ptr<CharacterCard> card);


	virtual ~Player();
private:
	std::string name;
	int age;

	bool isKing;
	bool turn;

	std::shared_ptr<CharacterCard> character1;
	std::shared_ptr<CharacterCard> character2;

	std::shared_ptr<Socket> client;

};

