#pragma once

#include "stdafx.h"
#include <bitset>
using namespace std;
#define KEYMAX 256

class KeyBoard
{
private:
	bitset<KEYMAX> up;
	bitset<KEYMAX> down;

public:
	KeyBoard();
	~KeyBoard();

	bool Up(int Key);
	bool Down(int Key);
	bool Jump(int Key);

};

