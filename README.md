# This repository is unmaintained
- The tools probably won't work with any recent version of the game
# All pre-built binaries have been removed from all releases
- there were reports that virus-total flagged the files as containing trojans
  - while I'm convinced these are false positives, I removed the binaries to reduce the risk of spreading malicious code
  - the binary-files were last built in 2020 using github-actions
    - I deleted the github-actions to not accidently rebuild any binaries
# P4GPC_Savegame_Helper
Tools to help with working with Persona 4 Golden (PC) savegames:

## 1. P4GPC_Savegame_Checksum_Updater
This tool calculates the md5 checksum of a dataXXXX.bin file and updates the corresponding dataXXXX.binslot file. Without this the savegame won't show up in the ingame menu after the savegame was modified, e.g. after editing the savefile or replacing it with a decrypted savegame from the PSVita version.

## 2. P4GPC_Namechanger
This let's you change the main character's name in a savegame that was saved from the PC version of the game at least once. If P4GPC_Savegame_Checksum_Updater.exe is found in the same directory it will also be called.
