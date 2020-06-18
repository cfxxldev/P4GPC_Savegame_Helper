// P4GPC_Namechanger.cpp: Definiert den Einstiegspunkt für die
// Anwendung.

#define _CRT_SECURE_NO_WARNINGS 1
#include <ctime>
#undef _CRT_SECURE_NO_WARNINGS

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <cstdio>
#include <filesystem>
#include <chrono>
#include <string>
#include <sstream>
#include <thread>
#include <iostream>
#include <algorithm> 

namespace fs = std::filesystem;
void backup(fs::path binslot_file)
	{
	auto backup_file = binslot_file;
	backup_file.remove_filename();
	backup_file /= "backup/";

	if (!fs::exists(backup_file))
		{
		fs::create_directories(backup_file);
		}
	if (fs::exists(backup_file))
		{
		backup_file /= binslot_file.filename();
		auto now = std::chrono::system_clock::now();
		auto time = std::chrono::system_clock::to_time_t(now);
		std::stringstream strExtension;
		strExtension << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S") << binslot_file.extension().generic_string();
		backup_file.replace_extension(strExtension.str());
		std::cout << "Creating backup: " << backup_file.make_preferred().string() << "\n";
		fs::copy_file(binslot_file, backup_file, fs::copy_options::overwrite_existing);
		}
	}

bool separate_console(void)
	{
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
		{
		printf("GetConsoleScreenBufferInfo failed: %lu\n", GetLastError());
		return false;
		}

		// if cursor position is (0,0) then we were launched in a separate console
	return ((!csbi.dwCursorPosition.X) && (!csbi.dwCursorPosition.Y));
	}

static inline void ltrim(std::string& s)
	{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char c)
									{
									return !isspace(c);
									}));
	}

static inline void rtrim(std::string& s)
	{
	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char c)
						 {
						 return !isspace(c) && c != 0;
						 }).base(), s.end());
	}

int main(int argc, char* argv[])
	{
	if (argc <= 0)
		return -1;

	bool wait_for_key = separate_console();

	std::cout << "Persona 4 Golden PC namechanger" << '\n';
	std::cout << "Copyright (c) 2020 Andreas Gebert" << "\n\n";

	fs::path exe_file(argv[0]);

	if (argc == 1)
		{
		wprintf(L"usage: %s dataXXXX.bin [lastname [firstname]]\n", exe_file.filename().c_str());
		wprintf(L"You can also just drag and drop a dataXXXX.bin file on this program.\n");
		if (wait_for_key)
			getchar();
		return 0;
		}

	exe_file = fs::absolute(exe_file);
	fs::current_path(exe_file.parent_path());

	fs::path bin_file(argv[1]);
	if (bin_file.extension() == ".bin" && fs::exists(bin_file))
		{
		std::string lastname	= (argc>=3)?argv[2]:"";
		std::string firstname	= (argc>=4)?argv[3]:"";
		if (lastname.empty())
			{
			std::cout << "please enter last name: ";
			std::getline(std::cin, lastname);
//			std::cin >> lastname;
			}
		if (firstname.empty())
			{
			std::cout << "please enter first name: ";
			std::getline(std::cin, firstname);
//			std::cin >> firstname;
			}

		// limit length to 8 Characters
		lastname.resize(8);
		firstname.resize(8);
		rtrim(lastname);
		rtrim(firstname);

		backup(bin_file);
		
		std::cout << "Setting character name to " << lastname << " " << firstname << "\n";
		FILE* fBin = nullptr;
		if (_wfopen_s(&fBin, bin_file.c_str(), L"r+b") == 0 && fBin)
			{
			char lastname_buffer[16];
			char firstname_buffer[16];
			memset(lastname_buffer, 0, sizeof lastname_buffer);
			memset(firstname_buffer, 0, sizeof firstname_buffer);
			strcpy_s(lastname_buffer, lastname.c_str());
			strcpy_s(firstname_buffer, firstname.c_str());
			// These locations are used by the PS-Vita version, the PC version updated these locations but ignores them otherwise
			if (fseek(fBin, 0x00010, SEEK_SET) == 0)
				{
				fwrite(lastname_buffer, 1, 8, fBin);
				}
			if (fseek(fBin, 0x00022, SEEK_SET) == 0)
				{
				fwrite(firstname_buffer, 1, 8, fBin);
				}
			if (fseek(fBin, 0x00064, SEEK_SET) == 0)
				{
				fwrite(lastname_buffer, 1, 8, fBin);
				}
			if (fseek(fBin, 0x00076, SEEK_SET) == 0)
				{
				fwrite(firstname_buffer, 1, 8, fBin);
				}

			// Calculate the checksum over the first 54 Bytes and save it at offset 0x00036(54)
			// this checksum is the reason the game crashes after using the Savegame editor to change the name
			if (fseek(fBin, 0x00000, SEEK_SET) == 0)
				{
				uint8_t buf[54];
				memset(buf, 0, sizeof buf);
				auto nCount = fread_s(buf, sizeof buf, sizeof uint8_t, _countof(buf), fBin);
				if (nCount == _countof(buf))
					{
					uint8_t checksum = 0;
					for (auto v : buf)
						{
						checksum += v;
						}
					if (fseek(fBin, 0x00036, SEEK_SET) == 0)
						{
						fwrite(&checksum, 1, 1, fBin);
						}
					}
				}

			// The PC uses these locations
			if (fseek(fBin, 0x019CE8, SEEK_SET) == 0)
				{
				uint8_t flag = 1;	// 0 = always use default name 
									// 1 = use character name from savefile
				fwrite(&flag, 1, 1, fBin);
				}

			if (fseek(fBin, 0x015130, SEEK_SET) == 0)
				{
				fwrite(lastname_buffer, 1, 8, fBin);
				}
			if (fseek(fBin, 0x015142, SEEK_SET) == 0)
				{
				fwrite(firstname_buffer, 1, 8, fBin);
				}
			if (fseek(fBin, 0x019CEC, SEEK_SET) == 0)
				{
				fwrite(lastname_buffer, 1, 8, fBin);
				}
			if (fseek(fBin, 0x019CFE, SEEK_SET) == 0)
				{
				fwrite(firstname_buffer, 1, 8, fBin);
				}

			fclose(fBin);
			}

		exe_file.replace_filename("P4GPC_Savegame_Checksum_Updater.exe");
		if (fs::exists(exe_file))
			{
			std::wcout << L"Calling " << exe_file.filename().wstring() << L" to fix up your checksum.\n";
			std::wstring cmd = exe_file.filename().wstring() + L" \"" + bin_file.make_preferred().wstring() + L"\"";
			std::wcout << cmd << L"\n\n";
			_wsystem(cmd.c_str());
			}
		else
			{
			std::cout << "Please make sure update the checksum in your dataXXXX.binslot file.\n";
			}
		}

	// Ignore to the end of file
	std::cin.clear();
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	if (wait_for_key)
		getchar();
	return 0;
	}
