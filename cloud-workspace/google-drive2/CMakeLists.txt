cmake_minimum_required(VERSION 3.16)
project(google_drive_sync_demo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Enable automatic moc/uic/rcc handling globally
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find Qt6 components
find_package(Qt6 REQUIRED COMPONENTS Widgets Network NetworkAuth)

# Create executable (list sources here)
add_executable(google_drive_sync_demo
    main.cpp
)

# Ensure the build directory is in include path so generated moc files (e.g., main.moc) are found
target_include_directories(google_drive_sync_demo PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

# Explicitly enable automoc/autorcc/autouic for the target (redundant but robust)
set_target_properties(google_drive_sync_demo PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
)

# Link Qt libraries
target_link_libraries(google_drive_sync_demo
    PRIVATE
        Qt6::Widgets
        Qt6::Network
        Qt6::NetworkAuth
)

# Optional: try to detect QtKeychain (prefer pkg-config), but do not fail if absent.
find_package(PkgConfig QUIET)

set(KEYCHAIN_FOUND OFF)
if(PkgConfig_FOUND)
    pkg_check_modules(_KEYCHAIN_QT6 QUIET Qt6Keychain)
    pkg_check_modules(_KEYCHAIN_QT5 QUIET Qt5Keychain)
    if(_KEYCHAIN_QT6_FOUND)
        set(KEYCHAIN_FOUND ON)
        set(KEYCHAIN_INCLUDE_DIRS ${_KEYCHAIN_QT6_INCLUDE_DIRS})
        set(KEYCHAIN_LIBRARIES ${_KEYCHAIN_QT6_LIBRARIES})
    elseif(_KEYCHAIN_QT5_FOUND)
        set(KEYCHAIN_FOUND ON)
        set(KEYCHAIN_INCLUDE_DIRS ${_KEYCHAIN_QT5_INCLUDE_DIRS})
        set(KEYCHAIN_LIBRARIES ${_KEYCHAIN_QT5_LIBRARIES})
    endif()
endif()

if(NOT KEYCHAIN_FOUND)
    find_library(_QTKEYCHAIN_LIB NAMES qt6keychain qt5keychain keychain PATHS /usr/lib /usr/lib64 /usr/local/lib /usr/local/lib64)
    if(_QTKEYCHAIN_LIB)
        set(KEYCHAIN_FOUND ON)
        set(KEYCHAIN_LIBRARIES ${_QTKEYCHAIN_LIB})
        find_path(_QTKEYCHAIN_INCLUDE NAMES keychain.h PATHS /usr/include /usr/local/include /usr/include/qt /usr/include/qt6 /usr/include/qt5)
        if(_QTKEYCHAIN_INCLUDE)
            set(KEYCHAIN_INCLUDE_DIRS ${_QTKEYCHAIN_INCLUDE})
        endif()
    endif()
endif()

if(KEYCHAIN_FOUND)
    message(STATUS "QtKeychain detected; linking and enabling QTKEYCHAIN_AVAILABLE")
    if(KEYCHAIN_INCLUDE_DIRS)
        target_include_directories(google_drive_sync_demo PRIVATE ${KEYCHAIN_INCLUDE_DIRS})
    endif()
    if(KEYCHAIN_LIBRARIES)
        target_link_libraries(google_drive_sync_demo PRIVATE ${KEYCHAIN_LIBRARIES})
    endif()
    target_compile_definitions(google_drive_sync_demo PRIVATE QTKEYCHAIN_AVAILABLE=1)
else()
    message(STATUS "QtKeychain not found; building without OS keyring support (fallback to encrypted file storage).")
endif()

# Install target (optional)
install(TARGETS google_drive_sync_demo
    RUNTIME DESTINATION bin
)

# Helpful compile flags for warnings (optional)
if (MSVC)
    target_compile_options(google_drive_sync_demo PRIVATE /W4 /permissive-)
else()
    target_compile_options(google_drive_sync_demo PRIVATE -Wall -Wextra -Wpedantic)
endif()
