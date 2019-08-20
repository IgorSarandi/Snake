#pragma once
#include "Objects.h"
class Field :
	public Objects
{
public:
	Field();
	~Field();

	std::pair<char, int> Figure() override;
};

