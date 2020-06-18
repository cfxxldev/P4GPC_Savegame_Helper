// P4GPC_Savegame_Checksum_Updater.cpp: Definiert den Einstiegspunkt für die
// Anwendung.

#define _CRT_SECURE_NO_WARNINGS 1
#include <ctime>
#undef _CRT_SECURE_NO_WARNINGS

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <cstdio>
#include "md5.h"
#include <filesystem>
#include <chrono>
#include <string>
#include <sstream>
#include <thread>
#include <iostream>

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


int main(int argc, char* argv[])
	{
	if (argc <= 0)
		return -1;

	bool wait_for_key = separate_console();
	std::cout << "Persona 4 Golden PC Savegame checksum updater" << '\n';
	std::cout << "Copyright (c) 2020 Andreas Gebert" << "\n\n";

	fs::path exe_file(argv[0]);

	if (argc == 1)
		{
		wprintf(L"usage: %s dataXXXX.bin [dataXXXX.bin ...]\n", exe_file.filename().c_str());
		wprintf(L"You can also just drag and drop a dataXXXX.bin (or multiple) file on this program.\n");
		if (wait_for_key)
			getchar();
		return 0;
		}

	exe_file = fs::absolute(exe_file);
	fs::current_path(exe_file.parent_path());

	for (int iFile = 1; iFile != argc; ++iFile)
		{
		fs::path bin_file(argv[iFile]);
		if (bin_file.extension() == ".bin" && fs::exists(bin_file))
			{
			fs::path binslot_file(bin_file);
			binslot_file.replace_extension(".binslot");
			if (fs::exists(binslot_file))
				{
				wprintf(L"processing: %s\n", bin_file.make_preferred().c_str());
				FILE* fBin = nullptr;
				if (_wfopen_s(&fBin, bin_file.c_str(), L"rb") == 0 && fBin)
					{
					MD5Context ctx{ 0 };
					MD5Init(&ctx);
					uint8_t	buf[256];
					do
						{
						auto nCount = fread_s(buf, sizeof buf, sizeof uint8_t, _countof(buf), fBin);
						if (nCount == 0)
							break;
						MD5Update(&ctx, buf, nCount);
						} while (!feof(fBin));
						fclose(fBin);

						unsigned char digest[17] = {0};
						if (!ferror(fBin))
							{
							MD5Final(digest, &ctx);
							wprintf(L"MD5: ");
							for (int iIndex = 0;iIndex < 16;++iIndex)
								{
								wprintf(L"%.2X", digest[iIndex]);
								}
							wprintf(L"\n");

							backup(binslot_file);

							wprintf(L"updating: %s\n", binslot_file.make_preferred().c_str());
							FILE* fBinSlot = nullptr;
							if (_wfopen_s(&fBinSlot, binslot_file.c_str(), L"r+b") == 0 && fBinSlot)
								{
								if (fseek(fBinSlot, 0x18, SEEK_SET) == 0)
									{
									fwrite(digest, 1, 16, fBinSlot);
									}
								fclose(fBinSlot);
								wprintf(L"done\n");
								}
							else
								{
								wprintf(L"error\n");
								}
							}
					}
				}
			else
				{
				wprintf(L"%s not found: skipping\n",binslot_file.make_preferred().c_str());
				}
			}
		}

	if (wait_for_key)
		getchar();
	return 0;
	}
