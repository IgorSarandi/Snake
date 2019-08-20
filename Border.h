#pragma once
#include "Objects.h"
class Border :
	public Objects
{
public:
	Border();
	~Border();

	std::pair<char, int> Figure() override;
};

