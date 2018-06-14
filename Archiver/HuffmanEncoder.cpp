#include "HuffmanEncoder.h"

HuffmanEncoder::HuffmanEncoder() : HuffmanAlgorithm() {
	start_info = 0;
}

bool HuffmanEncoder::init_outside_struct(string file_name)
{
	this->file_name = file_name;
	add_files_struct = new FileSystemNode(file_name);

	if (!add_files_struct->is_folder())
	{
		ifstream check(file_name);
		if (!check.good())
		{
			check.close();
			return false;
		}

		check.close();
		add_files_struct->add_file(FileInArchive(file_name));
		return true;
	}

#ifdef _WIN32
	add_files_struct->build_tree();
#elif __linux
	add_files_struct->build_tree_linux();
#endif
	return true;
}

bool HuffmanEncoder::set_file_name(string file_name)
{
	this->file_name = file_name;
	if (is_in_archive(file_name, archive_struct) != -1)
	{
		cout << "This file in archive: " << file_name << endl;
		return false;
	}
	return true;
}

void HuffmanEncoder::set_archive_name(string archive_name)
{
	this->archive_name = archive_name;
	ofstream stream(archive_name, ios::in);

	stream.seekp(0, ios::end);
	int pos = stream.tellp();
	if (pos == 0) {
		stream.close();
		return;
	}

	stream.close();
	init_archive_struct();
}

#define READ_BUFFER_SIZE 64 * 1024
void HuffmanEncoder::init_vector()
{
	ifstream stream(file_name, ios::binary | ios::in);
	string buffer;

	for (int i = 0; i < 256; i++)
		pairs[i].second = 0;

	int file_size = 0;
	while (!stream.eof())
	{
		buffer = "";
		for (int i = 0; i < READ_BUFFER_SIZE; i++)
		{
			char c = stream.get();
			if (stream.eof()) break;
			buffer += c;
		}
		file_size += buffer.size();
		int buffLength = buffer.size();
		for (int i = 0; i < buffLength; i++)
			pairs[(unsigned char)buffer[i]].second++;
	}
	stream.close();
}

void HuffmanEncoder::encode()
{
    encode_folder(add_files_struct);
	ofstream archive_stream(archive_name, ios::ate | ios::out | ios::in | ios::binary);

	//	cout << "\n\nEncoder\n\n";
	archive_struct->add_node(add_files_struct);

	//	cout << "Start info: " << start_info << endl;
	archive_stream.seekp(start_info, ios::beg);
	write_archive_struct(archive_struct, archive_stream);
	archive_stream << start_info;
	archive_stream.close();
	cout << "--------------- Encoding successfully completed ---------------" << endl;
}

void HuffmanEncoder::encode_folder(FileSystemNode *&node)
{
	vector<FileInArchive> files = node->get_list_of_files();
	int files_quantity = files.size();

	for (int i = 0; i < files_quantity; i++)
	{
		if (set_file_name(files[i].full_name))
		{
			files[i].pos_in_archive = start_info;
			cout << "----Encode " << files[i].full_name << "----" << endl;
			encode_one();
		}
		else {
			files.erase(files.begin() + i);
			files_quantity--;
			i--;
		}
	}
	node->set_list_of_files(files);
	vector<FileSystemNode*> sub_dir = node->get_sub_dir();
	int sub_dir_quantity = sub_dir.size();
	for (int i = 0; i < sub_dir_quantity; i++)
		encode_folder(sub_dir[i]);
}

void HuffmanEncoder::encode_one()
{
	init_vector();
	init_priority_queue();
	if (pair_queue.size() == 0) return;
	build_tree();

	ofstream write_stream(this->archive_name, ios::out | ios::binary | ios::ate | ios::in);
	ifstream stream(this->file_name, ios::in | ios::binary);

	write_stream.seekp(start_info, ios::beg);
	for (auto it = pairs.begin(); it != pairs.end(); it++)
	write_stream << it->second << " ";

	int read_buffer_size = 8;
	int bits_must_add = 0;
	string read_buffer;
	string write_buffer;
	string left_part;
	bool eof = false;

	while (!eof)
	{
		read_buffer.clear();
		int i = 0;
		for (; i < read_buffer_size; i++) {
			char c = stream.get();
			if (stream.eof()) break;
			read_buffer += c;
		}
		if (stream.eof())
			eof = true;

		write_buffer.clear();
		write_buffer = in_binary_string(read_buffer, left_part);

		int left = write_buffer.size() % 8;
		double size = 0;
		if (!eof)
		{
			left_part = write_buffer.substr(write_buffer.size() - left, left);
			size = (write_buffer.size()) / 8;
		}
		else
		{
			bits_must_add = (8 - left) % 8;
			for (int i = 0; i < bits_must_add + 1; i++)
			{
				write_buffer += "0";
			}
			size = write_buffer.size() / 8;
		}

		char* bytes = get_bytes(write_buffer);

		for (int i = 0; i < size; i++)
		{
			write_stream.put(bytes[i]);
		}
		delete[] bytes;
	}
	write_stream << bits_must_add << " ";
	start_info = write_stream.tellp();
	stream.close();
	write_stream.close();
}

string HuffmanEncoder::in_binary_string(string read_buffer, string left_part)
{
	string write_buffer;
	int buffer_size = read_buffer.size();

	for (int i = 0; i < buffer_size; i++)
	{
		int number = (unsigned char)read_buffer[i];
		write_buffer += codes[number];
	}
	return left_part + write_buffer;
}

char* HuffmanEncoder::get_bytes(string write_buffer)
{
	int degree_of_two[] = { 128, 64, 32, 16, 8, 4, 2, 1 };
	int bits_quantity = write_buffer.size() - write_buffer.size() % 8;

	char *bytes = new char[bits_quantity / 8];
	for (int i = 0; i < bits_quantity / 8; i++)
		bytes[i] = 0;

	int b = 0;
	for (int i = 0; i < bits_quantity; i += 8, b++)
	{
		for (int j = i; j < i + 8; j++)
		{
			if (write_buffer[j] == '1')
				bytes[b] += degree_of_two[j - i];
		}
	}
	return bytes;
}
