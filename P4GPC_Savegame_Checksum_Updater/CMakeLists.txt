# CMakeList.txt: CMake-Projekt für "P4GPC_Savegame_Checksum_Updater". Schließen Sie die Quelle ein, und definieren Sie
# projektspezifische Logik hier.
#
cmake_minimum_required (VERSION 3.8)
if(MSVC)
	add_compile_options(/MT$<$<CONFIG:Debug>:d>)
endif()

# Fügen Sie der ausführbaren Datei dieses Projekts eine Quelle hinzu.

add_executable (P4GPC_Savegame_Checksum_Updater "P4GPC_Savegame_Checksum_Updater.cpp" "P4GPC_Savegame_Checksum_Updater.h" "md5.c" "md5.h")

set_property(TARGET P4GPC_Savegame_Checksum_Updater PROPERTY CXX_STANDARD 17)

# TODO: Fügen Sie bei Bedarf Tests hinzu, und installieren Sie Ziele.
install(TARGETS P4GPC_Savegame_Checksum_Updater DESTINATION /)

include (CPack)