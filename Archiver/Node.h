#pragma once

#include <map>
#include <queue>
#include <string>
#include <iostream>
#include <fstream>

#include "PairCompare.h"

using namespace std;

class Node
{
public:

	Node *left;
	Node *right;
	pair<char, int> pairs;
	string code;
	bool is_digit;

	Node();
	Node(pair<char, int>);
	Node(pair<char, int>, Node*, Node*);

	void encodeHuffmanCodes(string, vector<string> &);
	void show(int);
};
