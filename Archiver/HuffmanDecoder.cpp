#include "HuffmanDecoder.h"

HuffmanDecoder::HuffmanDecoder() : HuffmanAlgorithm() {}

bool HuffmanDecoder::set_file_name(string file_name)
{
	this->file_name = file_name;
	must_decode = folder_in_archive(file_name, archive_struct);
	if (must_decode->is_folder())
    	return true;

	file_start_in_archive = is_in_archive(file_name, archive_struct);
	if (file_start_in_archive == -1)
	{
		cout << "File not found in archive";
		return false;
	}
	file_end_in_archive = get_end_pos(file_start_in_archive);
	must_decode = nullptr;
	return true;
}

bool HuffmanDecoder::set_archive_name(string archive_name)
{
	this->archive_name = archive_name;
	ifstream is_exist(archive_name.c_str());
	if(!is_exist.good()) return false;
	is_exist.close();
	init_archive_struct();
	archive_struct->show();
	return true;
}

void HuffmanDecoder::set_file_name_outside(string file_name_outside)
{
	this->file_name_outside = file_name_outside;
}

void HuffmanDecoder::init_vector()
{
	ifstream stream(archive_name.c_str(), ios::binary | ios::in);
	int quantity;

	stream.seekg(file_start_in_archive);
	pairs.clear();
	for (int i = 0; i < 256; i++)
	{
		stream >> quantity;
		pairs.push_back(pair<char, int>(i, quantity));
	}

	cout << file_start_in_archive << " ------------------: " << file_end_in_archive << endl;
	stream.seekg(file_end_in_archive - 1, ios::beg);
    bits_must_delete = stream.get() - '0';
	cout <<  "Bits must delete: " << bits_must_delete << endl << endl;
	stream.close();
}

void HuffmanDecoder::decode(bool just_delete)
{
	if (just_delete)
	{
		if (must_decode == nullptr) archive_struct->remove_file(file_name);
		else archive_struct->remove_folder(must_decode);
		rewrite_archive();
		return;
	}

	if (must_decode == nullptr)
	{
		cout << file_name;
		decode_one();
		archive_struct->remove_file(file_name);
		rewrite_archive();
	}
	else {
#ifdef _WIN32
		CreateDirectory(file_name_outside.c_str(), NULL);
#elif __linux
		struct stat st;
		if(stat(file_name_outside.c_str(), &st) == -1)
			mkdir(file_name_outside.c_str(), 0700);
#endif
		decode_folder(must_decode, file_name_outside + "/" + must_decode->get_name_in_folder());
		archive_struct->remove_folder(must_decode);
		rewrite_archive();
	}
	cout << "--------------- Decoding successfully completed ---------------" << endl;
}

void HuffmanDecoder::rewrite_archive()
{
	ifstream archive_read(archive_name.c_str(), ios::in | ios::binary);
	string memory = "";

	read_folder(archive_struct, archive_read, memory);
	archive_read.close();

	ofstream archive_write(archive_name.c_str(), ios::out | ios::binary);
	archive_write << memory;

	start_info = archive_write.tellp();
	write_archive_struct(archive_struct, archive_write);
	archive_write << " " << start_info;
	archive_write.close();
}

void HuffmanDecoder::read_folder(FileSystemNode *folder, ifstream& archive_read, string& memory)
{
	vector<FileInArchive> files = folder->get_list_of_files();
	for (vector<FileInArchive>::iterator it = files.begin(); it != files.end(); it++)
	{
		file_start_in_archive = it->pos_in_archive;
		file_end_in_archive = get_end_pos(file_start_in_archive);
		read_file(archive_read, memory);
	}

	vector<FileSystemNode*> sub_folders = folder->get_sub_dir();
	for (vector<FileSystemNode*>::iterator it = sub_folders.begin(); it != sub_folders.end(); it++)
		read_folder(*it, archive_read, memory);
}

void HuffmanDecoder::read_file(ifstream &stream, string& memory)
{
	stream.seekg(file_start_in_archive, ios::beg);
	for (int i = file_start_in_archive; i < file_end_in_archive; i++)
		memory += stream.get();
	memory += " ";
}

int HuffmanDecoder::get_end_pos(int start_pos)
{
	int i = 0;
	int vect_line_size = positions_in_line.size();
	for (; i < vect_line_size; i++)
		if (positions_in_line[i] == start_pos)
			break;

	if (i == vect_line_size - 1)
		return start_info - 1;
	return positions_in_line[i + 1] - 1;
}

void HuffmanDecoder::decode_folder(FileSystemNode* folder, string directory)
{
#ifdef _WIN32
	CreateDirectory(directory.c_str(), NULL);
#elif __linux
	struct stat st;
	cout << directory << endl;
	if(stat(directory.c_str(), &st) == -1)
	    mkdir(directory.c_str(), 0700);
#endif

	cout << folder->get_full_name() << endl;
	vector<FileSystemNode*> sub_dir = folder->get_sub_dir();
	for (auto it = sub_dir.begin(); it != sub_dir.end(); it++)
	{
		cout << directory << endl;
		decode_folder(*it, directory + "/" + (*it)->get_name_in_folder());
	}

	vector<FileInArchive> files = folder->get_list_of_files();
	for (auto it = files.begin(); it != files.end(); it++) {
		file_name = it->full_name;
		file_name_outside = directory + "/" + it->name_in_folder;

		file_start_in_archive = it->pos_in_archive;
		file_end_in_archive = get_end_pos(file_start_in_archive);

		ifstream test_exist(file_name_outside.c_str());

		if (test_exist.good())
		{
			string decide;
			cout << "File " << file_name_outside << " exist" << endl;
			cout << "Enter Yes to decode or any other key to skip" << endl;
			cin >> decide;
			cin.clear();

			if(decide == "Y" || decide == "y")
				decode_one();
			test_exist.close();
		}
		else
		{
			test_exist.close();
			decode_one();
		}

		folder->remove_file(it->full_name);
	}
}

#define BUFFER_SIZE 1024
void HuffmanDecoder::decode_one()
{
	init_vector();
	init_priority_queue();
	build_tree();

	ifstream archive_stream(archive_name.c_str(), ios::binary | ios::in);
	ofstream file_outside_stream(file_name_outside.c_str(), ios::binary | ios::out);

	string buffer, binary_str;
	bool eof = false;

	archive_stream.seekg(file_start_in_archive);
	int quantity = 0;
	for (int i = 0; i < 256; i++)
		archive_stream >> quantity;

	archive_stream.get();

	cout << "Decode..." << file_name << endl;
	while (!eof)
	{
		buffer.clear();
		for (int i = 0; i < BUFFER_SIZE; i++)
		{
			char c = archive_stream.get();
			int pos = archive_stream.tellg();
			if (pos == file_end_in_archive + 1)
			{
				eof = true;
#ifdef __linux
				buffer.erase(buffer.end() - 1);
#elif _WIN32
				buffer.pop_back();
#endif
				break;
			}
			buffer += c;
		}
		binary_str += in_binary_string(buffer);

		if (eof)
			for (int i = 0; i < 0; i++)
			{
#ifdef __linux
				buffer.erase(buffer.begin() + i);
#elif _WIN32
		        binary_str.pop_back();
#endif
			}

		string original = get_original_bytes(binary_str);
		int size = original.size();

		for (int i = 0; i < size; i++)
		{
			file_outside_stream.put(original[i]);
		}
	}
	archive_stream.close();
	file_outside_stream.close();
}

string HuffmanDecoder::in_binary_string(string bytes)
{
	string str_in_binary;
	int size = bytes.size();

	for (int i = 0; i < size; i++)
	{
		int digit = (unsigned char)bytes[i];
		string digit_in_binary = "00000000";
		for (int j = 0; j < 8 && digit; j++, digit /= 2) {
			digit_in_binary[7 - j] += digit % 2;
		}
		str_in_binary += digit_in_binary;
	}

	return str_in_binary;
}

string HuffmanDecoder::get_original_bytes(string &binary_string)
{
	string original_bytes;

	while (true)
	{
		int pos = 0;
		char c = get_character(parent, binary_string, pos);
		if (pos != 0)
		{
			binary_string = binary_string.substr(pos);
			original_bytes += c;
		}
		else break;
	}
	return original_bytes;
}

char HuffmanDecoder::get_character(Node *node, string binary_str, int& pos)
{
	if (!node->left && !node->right)
	{
		return node->pairs.first;
	}

	if (pos == binary_str.size() - 1)
	{
		pos = 0;
		return 0;
	}

	if (binary_str[pos] == '0') return get_character(node->left, binary_str, ++pos);
	return get_character(node->right, binary_str, ++pos);
}
