#pragma once
#include "Node.h"

Node::Node(pair<char, int> pairs, Node *left, Node *rigth)
{
	this->pairs = pairs;
	this->left = left;
	this->right = rigth;
	is_digit = true;
}

Node::Node(pair<char, int> pairs = pair<char, int>(0, 0))
{
	this->pairs = pairs;
	this->left = nullptr;
	this->right = nullptr;
	is_digit = true;
}

Node::Node() 
{
	left = nullptr;
	right = nullptr;
	pairs = pair<char, int>(0, 0);
	is_digit = true;
}

void Node::show(int start)
{
	if (left != nullptr) left->show(start + 5);
	for (int i = 0; i < start; i++) cout << " ";
	cout << (unsigned int) pairs.first << ":" << pairs.second << ":" << code << endl;
	if (right != nullptr) right->show(start + 5);
}

void Node::encodeHuffmanCodes(string code, vector<string> &map_of_codes) 
{
	if (is_digit)
	{
		int number = (unsigned char)pairs.first;
		map_of_codes[number] = code;
	}

	if (right == nullptr && left == nullptr)
	{
		this->code = code;
		return;
	}
	if (left != nullptr) left->encodeHuffmanCodes(code + '0', map_of_codes);
	if (right != nullptr) right->encodeHuffmanCodes(code + '1', map_of_codes);
	this->code = code;
}