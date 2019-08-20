#pragma once
#include "Objects.h"
#include <iostream>
#include <memory>

class Body :
	public Objects
{
public:
	Body();

	virtual ~Body();

	std::pair<int, int> GetDirection();

	void SetDirection(std::pair<int, int>);

	Body* GetParent();

	void SetParent(Body* _parent);

	std::pair<char, int> Figure() override;

protected:
	std::pair<int, int> direction { 0, 0 };
private:
	Body* parent;
};