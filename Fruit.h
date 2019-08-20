#pragma once
#include "Objects.h"
class Fruit :
	public Objects
{
public:
	Fruit();
	~Fruit();
	//random position

	std::pair<char, int> Figure() override;
};

