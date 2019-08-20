#pragma once
#include <iostream>

class Objects ///general abstract class
{
public:
	Objects();
	virtual ~Objects() = 0;

	void Destroy();

	void SetPosition(int _x, int _y);

	std::pair<int, int> GetPosition();

	virtual std::pair<char, int> Figure() = 0;

private:
	std::pair<int, int> position;
	int pos;
};

