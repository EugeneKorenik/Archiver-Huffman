#pragma once

#include "Node.h"
using namespace std;

struct NodeCompare
{
	bool operator()(Node *n1, Node *n2)
	{
		return n1->pairs.second > n2->pairs.second;
	}
};
