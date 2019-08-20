#include "Border.h"



Border::Border()
{
}


Border::~Border()
{
}

std::pair<char, int> Border::Figure()
{
	return std::make_pair('#', 10);
}