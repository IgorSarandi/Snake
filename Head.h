#pragma once
#include "Body.h"
class Head :
	public Body
{
public:

	Head(const Head&) = delete;

	Head& operator=(Head&) = delete;

	~Head();

	virtual std::pair<char, int> Figure() override;

	static Head& getSingleton();

	static void Eat();

	static int GetScore();

private:
	Head() {}
	static int score;
};