cmake_minimum_required(VERSION 3.10)
project(ExecLogger VERSION 1.0 LANGUAGES C)

# Name des Moduls (muss exakt zum .c Dateinamen passen!)
set(MODULE_NAME exec_logger)

# 1. Kernel-Informationen sammeln
execute_process(COMMAND uname -r OUTPUT_VARIABLE KERNEL_RELEASE OUTPUT_STRIP_TRAILING_WHITESPACE)
set(KERNEL_DIR "/lib/modules/${KERNEL_RELEASE}/build")

# 2. Das notwendige 'Makefile' für Kbuild dynamisch im Build-Verzeichnis erstellen
# Wichtig: Kbuild braucht die Syntax 'obj-m += name.o'
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/Kbuild "obj-m += ${MODULE_NAME}.o\n")

# 3. Das Haupt-Target
# Wir nutzen CMAKE_CURRENT_SOURCE_DIR für die .c Datei und 
# CMAKE_CURRENT_BINARY_DIR für das Kbuild-File und die Artefakte.
add_custom_target(${MODULE_NAME}
    ALL
    COMMAND make -C ${KERNEL_DIR} 
            M=${CMAKE_CURRENT_BINARY_DIR} 
            src=${CMAKE_CURRENT_SOURCE_DIR} 
            modules
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Kompiliere Kernel-Modul mit Kbuild..."
    VERBATIM
)

# 4. IDE-Support (Fix für Eclipse/Ninja)
# Wir fügen die Standardpfade manuell hinzu, da 'cflags-only' fehlgeschlagen ist.
if(EXISTS ${KERNEL_DIR})
    include_directories(
        SYSTEM ${KERNEL_DIR}/include
        SYSTEM ${KERNEL_DIR}/arch/x86/include
        SYSTEM ${KERNEL_DIR}/include/generated
        SYSTEM ${KERNEL_DIR}/arch/x86/include/generated
    )
    # Verhindert Fehlermeldungen in der IDE
    add_definitions(-D__KERNEL__ -DMODULE -DKBUILD_MODNAME="${MODULE_NAME}")
endif()