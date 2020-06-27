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
#include <fcntl.h>
#include <io.h>

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
		std::wcout << L"Creating backup: " << backup_file.make_preferred().wstring() << "\n";
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

int wmain(int argc, wchar_t* argv[])
	{
	if (argc <= 0)
		return -1;

	bool wait_for_key = separate_console();

	_setmode(_fileno(stdout), _O_U16TEXT);
	_setmode(_fileno(stdin), _O_U16TEXT);

	std::wcout << L"Persona 4 Golden PC Difficulty Menu Enabler" << L"\n";
	std::wcout << L"Copyright (c) 2020 Andreas Gebert" << L"\n\n";

	fs::path exe_file(argv[0]);

	if (argc == 1)
		{
		wprintf(L"usage: %s dataXXXX.bin\n", exe_file.filename().c_str());
		wprintf(L"You can also just drag and drop a dataXXXX.bin file on this program.\n");

		if (wait_for_key)
			getwchar();
		return 0;
		}

	exe_file = fs::absolute(exe_file);
	fs::current_path(exe_file.parent_path());

	fs::path bin_file(argv[1]);
	if (bin_file.extension() == ".bin" && fs::exists(bin_file))
		{
		backup(bin_file);
		
		FILE* fBin = nullptr;
		if (_wfopen_s(&fBin, bin_file.c_str(), L"r+b") == 0 && fBin)
			{
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

			// This flag controls the availability of the Difficulty menu ( and also most likely ng+ status)
			// observed values:
			//	00 - normal new game on vita
			//	01 - ng+ on vita (probably PC also)
			//	02 - normal new game on PC
			if (fseek(fBin, 0x01304, SEEK_SET) == 0)
				{
				uint8_t flag[1];
				auto nCount = fread_s(flag, sizeof flag, sizeof uint8_t, _countof(flag), fBin);
				if (nCount == _countof(flag) && flag[0] == 0x00)
					{
					flag[0] = 0x02;
					if (fseek(fBin, 0x01304, SEEK_SET) == 0)
						{
						fwrite(flag, sizeof uint8_t, _countof(flag), fBin);
						}
					}
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
			std::wcout << L"Please make sure update the checksum in your dataXXXX.binslot file.\n";
			}
		}

	// Ignore to the end of file
	std::wcin.clear();
	std::wcin.ignore(std::numeric_limits<std::streamsize>::max(), L'\n');

	if (wait_for_key)
		getwchar();
	return 0;
	}
