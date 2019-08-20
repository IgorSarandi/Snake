#include "Wall.h"



Wall::Wall()
{
}


Wall::~Wall()
{
}

std::pair<char, int> Wall::Figure()
{
	return std::make_pair('X', 15);
}