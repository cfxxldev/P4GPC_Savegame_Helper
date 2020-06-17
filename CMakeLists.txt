# CMakeList.txt: CMake-Projektdatei der obersten Ebene. Führen Sie hier die globale Konfiguration aus,
# und schließen Sie Unterprojekte ein.
#
cmake_minimum_required (VERSION 3.8)

project ("P4GPC_Savegame_Helper")

# Schließen Sie Unterprojekte ein.
add_subdirectory ("P4GPC_Savegame_Checksum_Updater")
add_subdirectory ("P4GPC_Namechanger")
