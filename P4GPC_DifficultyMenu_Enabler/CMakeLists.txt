﻿# CMakeList.txt: CMake-Projekt für "P4GPC_Namechanger". Schließen Sie die Quelle ein, und definieren Sie
# projektspezifische Logik hier.
#
cmake_minimum_required (VERSION 3.8)
if(MSVC)
	add_compile_options(/MT$<$<CONFIG:Debug>:d>)
endif()

# Fügen Sie der ausführbaren Datei dieses Projekts eine Quelle hinzu.

add_executable (P4GPC_DifficultyMenu_Enabler "P4GPC_DifficultyMenu_Enabler.cpp" "P4GPC_DifficultyMenu_Enabler.h")

set_property(TARGET P4GPC_DifficultyMenu_Enabler PROPERTY CXX_STANDARD 17)

# TODO: Fügen Sie bei Bedarf Tests hinzu, und installieren Sie Ziele.
install(TARGETS P4GPC_DifficultyMenu_Enabler DESTINATION /)

include (CPack)