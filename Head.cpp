#include "Head.h"

int Head::score = 0;

Head::~Head()
{
}

Head& Head::getSingleton()
{
	static Head singleton;
	singleton.SetDirection(std::make_pair(0, 0));
	return singleton;
}

std::pair<char, int> Head::Figure()
{
	return std::make_pair('O', 14);
}

void Head::Eat()
{
	score++;
}

int Head::GetScore()
{
	return score * 100;
}
