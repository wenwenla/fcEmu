cmake_minimum_required (VERSION 3.5)

PROJECT (FCEmu)
set (CMAKE_CXX_STANDARD 11)

# INCLUDE (FindPkgConfig)
# pkg_check_modules (SFML REQUIRED sfml-system sfml-window sfml-graphics)
# INCLUDE_DIRECTORIES (${SFML_INCLUDE_DIRS})
# LINK_DIRECTORIES (${SFML_LIBRARY_DIRS})

AUX_SOURCE_DIRECTORY (. SOURCES)


ADD_EXECUTABLE (EMU
    ${SOURCES}
)

TARGET_LINK_LIBRARIES (EMU
    sfml-system sfml-window sfml-graphics
)
