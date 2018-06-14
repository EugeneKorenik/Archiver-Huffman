#pragma once

#include "Node.h"
using namespace std;

struct PairCompare
{
	bool operator()(const pair<char, int> &p1, const pair<char, int> &p2)
	{
		return p1.second > p2.second;
	}
};
