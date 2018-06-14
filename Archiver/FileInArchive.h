#pragma once

#include <string>
using namespace std;

class FileInArchive
{
public:
	string full_name;
	string name_in_folder;
	int pos_in_archive;

	FileInArchive(string full_name)
	{
		this->full_name = full_name;

		int i = full_name.size() - 1;
		while(full_name[i] != '/' && i >= 0) i--;
		name_in_folder = full_name.substr(i + 1);

		this->pos_in_archive = 0;
	}

	FileInArchive(string full_name, string name_in_folder, int pos_in_archive)
	{
		this->full_name = full_name;
		this->name_in_folder = name_in_folder;
		this->pos_in_archive = pos_in_archive;
	}
};
