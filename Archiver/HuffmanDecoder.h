#pragma once

#include "HuffmanAlgorithm.h"

using namespace std;

class HuffmanDecoder : public HuffmanAlgorithm
{
	int file_start_in_archive;
	int file_end_in_archive;
	int bits_must_delete;
	string file_name_outside;

	vector<pair<string, int> > file_name_pos_vector;
	void init_vector();
	string in_binary_string(string);
	string get_original_bytes(string&);
	char get_character(Node*, string, int&);

	FileSystemNode *must_decode;
	void decode_folder(FileSystemNode*, string);
	void decode_one();
	int get_end_pos(int);

	void rewrite_archive();
	void read_folder(FileSystemNode*, ifstream&, string&);
	void read_file(ifstream&, string&);

public:

	HuffmanDecoder();
	bool set_archive_name(string);
	bool set_file_name(string);
	void set_file_name_outside(string);
	void decode(bool);
};

