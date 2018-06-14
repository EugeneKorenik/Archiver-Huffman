#pragma once

#include "HuffmanAlgorithm.h"
#include "FileSystemNode.h"

using namespace std;

class HuffmanEncoder : public HuffmanAlgorithm {

	string in_binary_string(string, string);
	FileSystemNode *add_files_struct;

	char* get_bytes(string); 
	void init_vector();
	void encode_one();
	void encode_folder(FileSystemNode *&);

public:

	HuffmanEncoder();
	bool set_file_name(string);
	bool init_outside_struct(string);
	void set_archive_name(string);
	void encode();
};
