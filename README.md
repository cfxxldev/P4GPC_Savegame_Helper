# P4GPC_Savegame_Helper
Tools to help with working with Persona 4 Golden (PC) savegames:

## 1. P4GPC_Savegame_Checksum_Updater
This tool calculates the md5 checksum of a dataXXXX.bin file and updates the corresponding dataXXXX.binslot file. Without this the savegame won't show up in the ingame menu after the savegame was modified, e.g. after editing the savefile or replacing it with a decrypted savegame from the PSVita version.

## 2. P4GPC_Namechanger
This let's you change the main character's name in a savegame that was saved from the PC version of the game at least once. If P4GPC_Savegame_Checksum_Updater.exe is found in the same directory it will also be called.
