#include "Objects.h"


Objects::Objects()
{
}

void Objects::Destroy()
{
	this->~Objects();
}

Objects::~Objects()
{
}

void Objects::SetPosition(int _x, int _y)
{
	position.first = _x;
	position.second = _y;
}

std::pair <int, int> Objects::GetPosition()
{
	return position;
}