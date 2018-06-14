#include "HuffmanEncoder.h"
#include "HuffmanDecoder.h"
#include "FileSystemNode.h"
#include <locale.h>

#ifdef _WIN32
#include <windows.h>
#elif __linux
#include <dirent.h>
#include <errno.h>
#endif

using namespace std;

void show_instruction()
{
	cout << "---------- HOW TO ENCODE ---------- " << endl;
	cout << "encode /home/user/myArchive.txt  /home/user/somefolder (or file) " << endl;

	cout << "---------- HOW TO DECODE ---------- " << endl;
	cout << "decode /home/user/myArchive.txt" << endl;
	cout << "Program show you a list of files in archive" << endl;
	cout << " /home/user/file_name_in_archive /home/user/file_name_outside" << endl;
}

void encode()
{
	string archive_name;
	string file_name_outside;
    HuffmanEncoder encoder;

	cout << "Enter archive_name: ";
    cin >> archive_name;
    archive_name += ".arch";
    encoder.set_archive_name(archive_name);

	while(true)
	{
		cout << "Enter file_name_outside (\"exit\" - to break): ";
		cin >> file_name_outside;
		if(file_name_outside == "exit") break;
		if(!encoder.init_outside_struct(file_name_outside))
		{
			cout << "File not found" << endl;
			continue;
		}
		encoder.encode();
		break;
	}
}

void decode(bool just_delete)
{
	string file_name;
	string file_name_outside;
	string archive_name;
	HuffmanDecoder decoder;

	while(true)
	{
		cout << "Enter archive name (\"exit\" - to break): ";
		cin >> archive_name;
		cin.clear();
		archive_name += ".arch";
		if(archive_name == "exit.arch") return;
		if(decoder.set_archive_name(archive_name)) break;
		cout << "Such archive not found" << endl;
	}

	while(true)
	{
		cout << "Enter file name in archive (\"exit\" - to break): ";
		cin >> file_name;
		if(file_name == "exit") return;
		if(decoder.set_file_name(file_name)) break;
		cout << "Such file not found in archive" << endl;
	}

	if (!just_delete)
	{
		cout << "Enter directory to unpack file from archive (\"exit\" - to break): ";
		cin >> file_name_outside;
		if (file_name_outside == "exit") return;
		decoder.set_file_name_outside(file_name_outside);
	}
	decoder.decode(just_delete);
}

int main()
{
	string buffer;
	while(true)
	{
		cout << "Enter operation: (encode / decode / delete / exit / clear (screen)): ";
		cin >> buffer;
		cin.clear();
		if(buffer == "exit") return 0;
		else if(buffer == "encode") encode();
		else if(buffer == "decode") decode(false);
		else if (buffer == "delete") decode(true);
		else if (buffer == "clear")
		{
#ifdef _WIN32
			system("cls");
#elif
			cout << "\033[H\033[J";
#endif
		}
		else cout << "Wring operation " << endl;
	}
	return 0;
}
