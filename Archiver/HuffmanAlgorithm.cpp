#include "HuffmanAlgorithm.h"

HuffmanAlgorithm::HuffmanAlgorithm()
{
	archive_struct = new FileSystemNode();
	pairs.clear();
	for (int i = 0; i < 256; i++)
		pairs.push_back(pair<char, int>(i, 0));
}

#define MAX_DIGIT_SIZE 12
void HuffmanAlgorithm::init_archive_struct()
{
	ifstream archive_stream(archive_name, ios::in | ios::ate | ios::binary);
	if (!archive_stream.is_open())
	{
		archive_stream.close();
		ofstream create_file(archive_name);
		create_file.close();
		start_info = 0;
		return;
	}

	archive_stream.seekg(0, ios::end);
	int end_of_file = archive_stream.tellg();
	if (end_of_file == 2) return;

	string digit;
	archive_stream.seekg((-1) * MAX_DIGIT_SIZE, ios::end);
	for (int i = 0; i < MAX_DIGIT_SIZE; i++)
	{
		char c = archive_stream.get();
		if (archive_stream.eof()) break;
		if (c == ' ') digit.clear();
		else digit += c;
	}

	start_info = atoi(digit.c_str());
	archive_stream.seekg(start_info, ios::beg);
	positions_in_line.clear();

	archive_struct = new FileSystemNode();
	build_struct(archive_struct, archive_stream);
	archive_stream.close();
}

void HuffmanAlgorithm::build_struct(FileSystemNode *&node, ifstream & archive_stream) 
{
	setlocale(LC_ALL, "RUS");
	while (true)
	{
		string buffer;
		int pos;
	
		archive_stream >> buffer;
		if (archive_stream.eof()) break;
		if (buffer == "<FOLDER_END>") break;
		if (buffer == "<FOLDER>")
		{
			archive_stream >> buffer;
			FileSystemNode* new_node = new FileSystemNode(buffer);
			node->add_folder(new_node);
			build_struct(new_node, archive_stream);
			continue;
		}
		archive_stream >> pos;
		positions_in_line.push_back(pos);
		if(!node->get_full_name().empty()) {
		    node->add_file(FileInArchive(node->get_full_name() + "/" + buffer, buffer, pos));
		}
		else
			node->add_file(FileInArchive(buffer));
	}
}

FileSystemNode* HuffmanAlgorithm::folder_in_archive(string folder_name, FileSystemNode* node)
{
	vector<FileSystemNode*> folders = node->get_sub_dir();
	for (auto it = folders.begin(); it != folders.end(); it++)
	{
		if (folder_name == (*it)->get_full_name()) return *it;
		FileSystemNode* found_node = folder_in_archive(folder_name, *it);
		if (found_node->is_folder()) return found_node;
	}
	FileSystemNode *not_found_folder = new FileSystemNode("", "", false);
	return not_found_folder;
}

int HuffmanAlgorithm::is_in_archive(string file_name, FileSystemNode *node)
{
	vector<FileInArchive> files_vect = node->get_list_of_files();
	for (auto it = files_vect.begin(); it != files_vect.end(); it++)
	{
		if (it->full_name == file_name)
			return it->pos_in_archive;
	}

	vector<FileSystemNode*> sub_dir = node->get_sub_dir();
	for (auto it = sub_dir.begin(); it != sub_dir.end(); it++) {
		int result =  is_in_archive(file_name, *it);
		if (result != -1) return result;
	}
	return -1;
}

void HuffmanAlgorithm::init_priority_queue()
{
	while(!pair_queue.empty()) pair_queue.pop();

	for (auto it = pairs.begin(); it != pairs.end(); it++)
		pair_queue.push(*it);

	while (true)
	{
		if (pair_queue.empty()) break;
		pair<char, int> temp = pair_queue.top();
		if (temp.second == 0) pair_queue.pop();
		else break;
	}
}

void HuffmanAlgorithm::build_tree()
{
	int size = pair_queue.size();

	while (!node_queue.empty())
		node_queue.pop();

	for (int i = 0; i < size; i++) {
		node_queue.push(new Node(pair_queue.top()));
		pair_queue.pop();
	}

	if (size == 1)
	{
		parent->left = node_queue.top();
		node_queue.pop();
		node_queue.push(parent);
	}

	for (int i = 0; i < size - 1; i++)
	{
		Node *left = node_queue.top();
		node_queue.pop();
		Node *right = node_queue.top();
		node_queue.pop();

		Node *parent = new Node(pair<char, int>(0,
			left->pairs.second + right->pairs.second));
		parent->is_digit = false;

		parent->left = left;
		parent->right = right;

		node_queue.push(parent);
	}
	parent = node_queue.top();

	for (int i = 0; i < 256; i++)
		codes.push_back("");

	parent->encodeHuffmanCodes("", codes);
}

void HuffmanAlgorithm::write_archive_struct(FileSystemNode *node, ofstream &write_stream)
{
	vector<FileInArchive> files = node->get_list_of_files();
	vector<FileSystemNode*> children = node->get_sub_dir();

	if (node->is_folder())
		write_stream << " <FOLDER> " << node->get_full_name() << " ";

	if(!node->get_full_name().empty())
		for (auto it = files.begin(); it != files.end(); it++)
			write_stream << it->name_in_folder << " " << it->pos_in_archive << " ";
	else
		for(auto it = files.begin(); it != files.end(); it++)
		    write_stream << it->full_name << " " << it->pos_in_archive << " ";

	for (auto it = children.begin(); it != children.end(); it++)
		write_archive_struct(*it, write_stream);

	if(node->is_folder())
	    write_stream << " <FOLDER_END> ";
}

void HuffmanAlgorithm::set_codes(vector<string> codes) { this->codes = codes; }
Node* HuffmanAlgorithm::get_parent() { return parent; }
vector<pair<char, int> > HuffmanAlgorithm::get_pairs_vector() { return pairs; }
vector<string> HuffmanAlgorithm::get_codes() { return codes; }
