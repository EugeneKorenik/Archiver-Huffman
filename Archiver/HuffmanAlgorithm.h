#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <map>

#ifdef _WIN32
#include <locale.h>
#endif

#include "Node.h"
#include "PairCompare.h"
#include "NodeCompare.h"
#include "FileSystemNode.h"
#include <stdlib.h>

using namespace std;

class HuffmanAlgorithm
{
protected:
	string archive_name;
	string file_name;
	int start_info;

	FileSystemNode *archive_struct;
	vector<int> positions_in_line;
	vector<pair<char, int> > pairs;
	priority_queue<pair<char, int>, vector<pair<char, int > >, PairCompare > pair_queue;
	priority_queue<Node*, vector<Node*>, NodeCompare> node_queue;
	vector<string> codes;

	Node* parent;

	void init_priority_queue();
	void build_struct(FileSystemNode*&, ifstream &);
	void build_tree();
	void init_archive_struct();
	void write_archive_struct(FileSystemNode *, ofstream &);
	int is_in_archive(string, FileSystemNode*);
	FileSystemNode* folder_in_archive(string, FileSystemNode*);
	
public:
	
	HuffmanAlgorithm();
	void set_codes(vector<string>);
	vector<string> get_codes();

	Node *get_parent();
	vector<pair<char, int > > get_pairs_vector();

};
