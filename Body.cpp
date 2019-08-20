#include "Body.h"


Body::Body()
{
}

Body::~Body()
{
}

std::pair<int, int> Body::GetDirection()
{
	return direction;
}

Body* Body::GetParent()
{
	return parent;
}

void Body::SetParent(Body* _parent)
{
	parent = _parent;
}

/// 1 - int is previous direction
/// 2 - int is current direction
void Body::SetDirection(std::pair<int, int> _dir)
{
	direction = _dir;
}

std::pair<char, int> Body::Figure()
{
	return std::make_pair('o', 14);
}