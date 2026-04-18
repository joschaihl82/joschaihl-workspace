cmake_minimum_required(VERSION 3.16)
project(cia-antivirus LANGUAGES CXX)

# --- Basic settings ---
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Enable Qt automoc/auto features
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# --- Find Qt6 (Widgets + WebEngineWidgets) ---
find_package(Qt6 COMPONENTS Widgets WebEngineWidgets REQUIRED)

# --- Try pkg-config for libclamav and alsa (preferred) ---
find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(PC_CLAMAV QUIET libclamav)
    pkg_check_modules(PC_ALSA QUIET alsa)
endif()

# --- Fallbacks for libclamav ---
if(PC_CLAMAV_FOUND)
    set(CLAMAV_LIBRARIES ${PC_CLAMAV_LIBRARIES})
    set(CLAMAV_INCLUDE_DIRS ${PC_CLAMAV_INCLUDE_DIRS})
    link_directories(${PC_CLAMAV_LIBRARY_DIRS})
else()
    find_path(CLAMAV_INCLUDE_DIR NAMES clamav.h PATHS /usr/include /usr/include/clamav /usr/include/x86_64-linux-gnu/qt6 NO_DEFAULT_PATH)
    find_library(CLAMAV_LIBRARY NAMES clamav libclamav PATHS /usr/lib /usr/lib/x86_64-linux-gnu /usr/local/lib)
    if(CLAMAV_LIBRARY)
        set(CLAMAV_LIBRARIES ${CLAMAV_LIBRARY})
    endif()
    if(CLAMAV_INCLUDE_DIR)
        set(CLAMAV_INCLUDE_DIRS ${CLAMAV_INCLUDE_DIR})
    endif()
endif()

# --- Fallbacks for ALSA ---
if(PC_ALSA_FOUND)
    set(ALSA_LIBRARIES ${PC_ALSA_LIBRARIES})
    set(ALSA_INCLUDE_DIRS ${PC_ALSA_INCLUDE_DIRS})
    link_directories(${PC_ALSA_LIBRARY_DIRS})
else()
    find_path(ALSA_INCLUDE_DIR NAMES alsa/asoundlib.h PATHS /usr/include /usr/include/alsa /usr/include/x86_64-linux-gnu NO_DEFAULT_PATH)
    find_library(ALSA_LIBRARY NAMES asound snd PATHS /usr/lib /usr/lib/x86_64-linux-gnu /usr/local/lib)
    if(ALSA_LIBRARY)
        set(ALSA_LIBRARIES ${ALSA_LIBRARY})
    endif()
    if(ALSA_INCLUDE_DIR)
        set(ALSA_INCLUDE_DIRS ${ALSA_INCLUDE_DIR})
    endif()
endif()

# --- Source files ---
set(SRC
    cia-antivirus4.cpp
)

# --- Optional resource file (icons) ---
# Create resources/icons.qrc with your icons or adjust path below.
set(RESOURCE_FILE resources/icons.qrc)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${RESOURCE_FILE}")
    list(APPEND SRC ${RESOURCE_FILE})
endif()

# --- Create executable ---
add_executable(${PROJECT_NAME} ${SRC})

# --- Link Qt targets ---
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets Qt6::WebEngineWidgets)

# --- Link libclamav and alsa if found ---
if(CLAMAV_LIBRARIES)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${CLAMAV_LIBRARIES})
    if(CLAMAV_INCLUDE_DIRS)
        target_include_directories(${PROJECT_NAME} PRIVATE ${CLAMAV_INCLUDE_DIRS})
    endif()
else()
    message(WARNING "libclamav not found. Install libclamav-dev or ensure pkg-config can find libclamav.")
endif()

if(ALSA_LIBRARIES)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${ALSA_LIBRARIES})
    if(ALSA_INCLUDE_DIRS)
        target_include_directories(${PROJECT_NAME} PRIVATE ${ALSA_INCLUDE_DIRS})
    endif()
else()
    message(WARNING "ALSA (libasound) not found. Install libasound2-dev or ensure pkg-config can find alsa.")
endif()

# --- Threads ---
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

# --- Include project dir and any pkg-config include dirs ---
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
if(PC_CLAMAV_FOUND)
    target_include_directories(${PROJECT_NAME} PRIVATE ${PC_CLAMAV_INCLUDE_DIRS})
endif()
if(PC_ALSA_FOUND)
    target_include_directories(${PROJECT_NAME} PRIVATE ${PC_ALSA_INCLUDE_DIRS})
endif()

# --- Compiler warnings ---
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /permissive-)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# --- RPATH handling (useful on Linux) ---
set_target_properties(${PROJECT_NAME} PROPERTIES
    BUILD_WITH_INSTALL_RPATH ON
    INSTALL_RPATH "$ORIGIN/../lib"
)

# --- Install target ---
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

# --- Helpful status messages ---
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
if(PC_CLAMAV_FOUND)
    message(STATUS "libclamav (pkg-config) found: ${PC_CLAMAV_LIBRARIES}")
elseif(CLAMAV_LIBRARIES)
    message(STATUS "libclamav found via find_library: ${CLAMAV_LIBRARIES}")
endif()
if(PC_ALSA_FOUND)
    message(STATUS "alsa (pkg-config) found: ${PC_ALSA_LIBRARIES}")
elseif(ALSA_LIBRARIES)
    message(STATUS "alsa found via find_library: ${ALSA_LIBRARIES}")
endif()
message(STATUS "Qt6 Widgets and WebEngineWidgets: ${Qt6Widgets_FOUND}, ${Qt6WebEngineWidgets_FOUND}")
