﻿# CMakeList.txt: CMake-Projektdatei der obersten Ebene. Führen Sie hier die globale Konfiguration aus,
# und schließen Sie Unterprojekte ein.
#
cmake_minimum_required (VERSION 3.8)

project ("P4GPC_Savegame_Helper" VERSION 1.0.2)

# Schließen Sie Unterprojekte ein.
add_subdirectory ("P4GPC_Savegame_Checksum_Updater")
add_subdirectory ("P4GPC_Namechanger")
add_subdirectory ("P4GPC_DifficultyMenu_Enabler")

install(FILES "LICENSE.txt" "readme.txt"
    DESTINATION .
)

SET (CPACK_GENERATOR "ZIP")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
include (CPack)