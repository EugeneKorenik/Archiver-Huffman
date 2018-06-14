#pragma once
#include <vector>
#include <iostream>
#include <map>
#include <string>
#include <fstream>

#ifdef _WIN32
#include <windows.h>
#elif __linux
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#endif

#include "FileInArchive.h"

using namespace std;

class FileSystemNode
{
	bool folder;
	string full_name;
	string name_in_folder;

	vector<FileSystemNode*> sub_dir;
	vector<FileInArchive> files;
	void define_is_directory();

#ifdef __linux
	void build_tree_linux_step(DIR*, dirent*);
#endif

	string delete_tail(string);
public:

	FileSystemNode();
	FileSystemNode(string);
	FileSystemNode(string, string, bool);
	~FileSystemNode();

	vector<FileSystemNode*> get_sub_dir();
	vector<FileInArchive> get_list_of_files();
	void set_list_of_files(vector<FileInArchive>);
	
	string get_full_name();
	string get_name_in_folder();

	bool is_folder();
	void set_file_name(string);

	void add_node(FileSystemNode*);

	void add_file(FileInArchive);
	void add_folder(FileSystemNode*);

	void remove_file(string);
	void remove_folder(FileSystemNode*);

	void build_tree();
	void show();
	void build_tree_linux();

	void define_is_directory_linux();
}; 
