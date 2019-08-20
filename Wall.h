#pragma once
#include "Objects.h"
class Wall :
	public Objects
{
public:
	Wall();
	~Wall();

	std::pair<char, int> Figure() override;
};

