#include "FileSystemNode.h"

FileSystemNode::FileSystemNode() 
{
	folder = false;
}

FileSystemNode::~FileSystemNode()
{
	for(auto it = sub_dir.begin(); it != sub_dir.end(); it++)
		delete *it;
}

FileSystemNode::FileSystemNode(string full_name)
{
	this->full_name = full_name;
	
	int i = full_name.size() - 1;
	while(full_name[i] != '/' && i >= 0) i--;
	name_in_folder = full_name.substr(i + 1);

#ifdef _WIN32
	define_is_directory();
#elif __linux
	define_is_directory_linux();
#endif
}

FileSystemNode::FileSystemNode(string full_name, string name_in_folder, bool folder)
{
	this->full_name = full_name;
	this->name_in_folder = name_in_folder;
	this->folder = folder;
}

void FileSystemNode::set_file_name(string file_name) {
	this->full_name = full_name;
#ifdef _WIN32
	define_is_directory();
#elif __linux
    define_is_directory_linux();
#endif
}

void FileSystemNode::set_list_of_files(vector<FileInArchive> files) { this->files = files; }

vector<FileInArchive> FileSystemNode::get_list_of_files() { return files; }

vector<FileSystemNode*> FileSystemNode::get_sub_dir() { return sub_dir; }

void FileSystemNode::add_file(FileInArchive file) { files.push_back(file); }
void FileSystemNode::add_folder(FileSystemNode* folder) { sub_dir.push_back(folder); }

string FileSystemNode::delete_tail(string path)
{
	int i = path.size() - 1;
	while(path[i] != '/') i--;
	return path.substr(0, i);
}

void FileSystemNode::add_node(FileSystemNode* folder)
{
	string name = folder->full_name;
//	cout << "\n-----" << name << "-----" << endl;
	for(auto it = sub_dir.begin(); it != sub_dir.end(); it++)
	{
		if((*it)->full_name == folder->full_name)
		{
			vector<FileInArchive> files_in_folder = folder->files;
			vector<FileSystemNode*> sub_dir_in_folder = folder->sub_dir;

			for(auto it = files_in_folder.begin(); it != files_in_folder.end(); it++)
				files.push_back(*it);

			for(auto it = sub_dir_in_folder.begin(); it != sub_dir_in_folder.end(); it++)
				sub_dir.push_back(*it);

			return;
		}

		if(name == (*it)->full_name)
		{
			(*it)->add_node(folder);
			return;
		}
		name = delete_tail(name);
	}
	sub_dir.push_back(folder);
}

bool FileSystemNode::is_folder() { return folder; }

string FileSystemNode::get_full_name() { return full_name; }

string FileSystemNode::get_name_in_folder() { return name_in_folder; }

void FileSystemNode::remove_folder(FileSystemNode* folder)
{
	for (auto it = sub_dir.begin(); it != sub_dir.end(); it++)
	{
		if (*it == folder)
		{
			sub_dir.erase(it);
			break;
		}
		else
			(*it)->remove_folder(folder);		
	}
}

void FileSystemNode::remove_file(string file_name)
{
	for(vector<FileInArchive>::iterator it = files.begin(); it != files.end(); it++)
		if (file_name == it->full_name)
		{
			files.erase(it);
			break;
		}
}

#ifdef _WIN32
void FileSystemNode::define_is_directory()
{
	WIN32_FIND_DATA find_data;
	HANDLE handle = FindFirstFile(full_name.c_str(), &find_data);
	folder = find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
	FindClose(handle);
}

void FileSystemNode::build_tree()
{
	setlocale(LC_ALL, "Rus");
	WIN32_FIND_DATA find_data;
	string path = full_name + "/*.*";

	HANDLE handle = FindFirstFile(path.c_str(), &find_data);
	do
	{
		string name = full_name + "/" + find_data.cFileName;
		if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (!strcmp(find_data.cFileName, ".") || !strcmp(find_data.cFileName, "..")) continue;
			FileSystemNode *node = new FileSystemNode(name, find_data.cFileName, true);
			sub_dir.push_back(node);
		}
		else
		{
			ifstream check(name);
			check.seekg(0, ios::end);
			int end = check.tellg();
			check.close();
			if(end)
				files.push_back(FileInArchive(name, find_data.cFileName, 0));
		}

	} while (FindNextFile(handle, &find_data));

	int sub_dir_quantity = sub_dir.size();
	cout << "--------------------" << full_name << "-----------" << endl;
	for (auto it = files.begin(); it != files.end(); it++)
		cout << it->full_name << endl;	

	for (int i = 0; i < sub_dir_quantity; i++)
		sub_dir[i]->build_tree();
	FindClose(handle);
}

#elif __linux

void FileSystemNode::build_tree_linux()
{
	DIR *dp;
	dp = opendir(full_name.c_str());
	if(ENOENT == errno)
	{
		cout << "Not found" << endl;
		getchar();
		return;
	}

	struct dirent *direct;
	while((direct = readdir(dp)) != NULL)
	{
		if(!strcmp(direct->d_name, ".") || !strcmp(direct->d_name, "..")
				|| !strcmp(direct->d_name, ".metadata")) continue;

		if(direct->d_type == DT_DIR)
		{
			FileSystemNode* node = new FileSystemNode(full_name + "/" + direct->d_name, direct->d_name, true);
			sub_dir.push_back(node);
			node->build_tree_linux();
		}
		else
		{
			ifstream check(full_name + "/" + direct->d_name);
			check.seekg(0, ios::end);
			int end = check.tellg();
			check.close();
			if(end)
			    files.push_back(FileInArchive(full_name + "/" + direct->d_name, direct->d_name, true));
		}
	}
	closedir(dp);
}

void FileSystemNode::define_is_directory_linux()
{
    struct stat st;
    if(stat(full_name.c_str(), &st) == 0)
    	folder = S_ISDIR(st.st_mode);
}
#endif

void FileSystemNode::show()
{
	for (auto it = files.begin(); it != files.end(); it++)
		cout << it->full_name << endl;

	for (auto it = sub_dir.begin(); it != sub_dir.end(); it++) {
		cout << (*it)->full_name << endl;
		(*it)->show();
	}
}