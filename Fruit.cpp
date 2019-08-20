#include "Fruit.h"



Fruit::Fruit()
{
}


Fruit::~Fruit()
{
}

std::pair<char, int> Fruit::Figure()
{
	return std::make_pair('*', 12);
}