cmake_minimum_required(VERSION 3.10)
project(exectrace1_module NONE)

# Module configuration (override with -D if desired)
set(MODULE_NAME "exectrace1" CACHE STRING "Kernel module base name (without .ko)")
set(MODULE_SRC "exectrace1.c" CACHE STRING "Module source file")
set(MODULE_OUTPUT_DIR "${CMAKE_BINARY_DIR}/out" CACHE PATH "Directory to place built .ko")

# Auto-generate a minimal Makefile for the kernel build if none exists
if(NOT EXISTS "${CMAKE_SOURCE_DIR}/Makefile")
  file(WRITE "${CMAKE_SOURCE_DIR}/Makefile" "obj-m := ${MODULE_NAME}.o\n")
  message(STATUS "No Makefile found in source dir — wrote minimal Makefile with obj-m := ${MODULE_NAME}.o")
endif()

# Auto-detect kernel build directory
if(NOT DEFINED KERNEL_DIR OR NOT EXISTS "${KERNEL_DIR}")
  execute_process(COMMAND uname -r
                  OUTPUT_VARIABLE UNAME_R
                  OUTPUT_STRIP_TRAILING_WHITESPACE)

  set(_candidates
      "/lib/modules/${UNAME_R}/build"
      "/usr/src/linux-headers-${UNAME_R}"
      "/usr/src/kernels/${UNAME_R}"
      "/lib/modules/${UNAME_R}/source"
      "/usr/src/linux"
  )

  set(_found_kernel_dir "")
  foreach(_p IN LISTS _candidates)
    if(EXISTS "${_p}")
      set(_found_kernel_dir "${_p}")
      break()
    endif()
  endforeach()

  if(_found_kernel_dir STREQUAL "")
    message(FATAL_ERROR "Could not auto-detect kernel build directory.
Set -DKERNEL_DIR=/path/to/kernel/build (e.g. /lib/modules/$(uname -r)/build) and re-run cmake.")
  endif()

  set(KERNEL_DIR "${_found_kernel_dir}" CACHE PATH "Path to kernel build directory" FORCE)
endif()

# Ensure a make program is available
if(NOT CMAKE_MAKE_PROGRAM)
  find_program(CMAKE_MAKE_PROGRAM make REQUIRED)
endif()

# Primary build target (default)
add_custom_target(kbuild ALL
  COMMENT "Building kernel module ${MODULE_NAME} using kernel build system at ${KERNEL_DIR}"
)

# Run kernel build verbosely and copy .ko only if produced; otherwise print diagnostics.
add_custom_command(TARGET kbuild
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory "${MODULE_OUTPUT_DIR}"
  COMMAND /bin/sh -c
    "echo '=== Invoking kernel build: ${CMAKE_MAKE_PROGRAM} -C \"${KERNEL_DIR}\" M=\"${CMAKE_SOURCE_DIR}\" modules V=1 ==='; \
     ${CMAKE_MAKE_PROGRAM} -C \"${KERNEL_DIR}\" \"M=${CMAKE_SOURCE_DIR}\" modules V=1 2>&1 | tee build_kernel.log; \
     if [ -f '${CMAKE_SOURCE_DIR}/${MODULE_NAME}.ko' ]; then \
       echo '=== Build succeeded: copying ${MODULE_NAME}.ko to ${MODULE_OUTPUT_DIR} ==='; \
       ${CMAKE_COMMAND} -E copy_if_different '${CMAKE_SOURCE_DIR}/${MODULE_NAME}.ko' '${MODULE_OUTPUT_DIR}/${MODULE_NAME}.ko'; \
     else \
       echo '=== ERROR: ${MODULE_NAME}.ko not found after kernel build ===' >&2; \
       echo; \
       echo '--- Quick diagnostic checklist ---' >&2; \
       echo '1) Inspect build log: build_kernel.log' >&2; \
       echo '2) Ensure linux-headers for this kernel are installed and KERNEL_DIR is correct:' >&2; \
       echo \"   uname -r => $(uname -r)\" >&2; \
       echo \"   detected KERNEL_DIR=${KERNEL_DIR}\" >&2; \
       echo '3) Ensure source contains: obj-m := ${MODULE_NAME}.o (a minimal Makefile was created if missing)' >&2; \
       echo '4) Re-run the kernel build manually for full context:' >&2; \
       echo \"   make -C ${KERNEL_DIR} M=${CMAKE_SOURCE_DIR} modules V=1 2>&1 | tee build_kernel.log\" >&2; \
       echo; \
       echo '--- First matching error lines from build_kernel.log ---' >&2; \
       awk '\''/error:|undefined reference|fatal error/ {p=1} p{print; c++} c>200{exit}'\'' build_kernel.log 2>/dev/null || true; \
       echo; \
       tail -n 80 build_kernel.log >&2 || true; \
       exit 1; \
     fi"
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMENT "Invoking kernel build (verbose) and performing automated diagnosis if needed"
  VERBATIM
)

# Auxiliary convenience targets
add_custom_target(kclean
  COMMAND ${CMAKE_MAKE_PROGRAM} -C "${KERNEL_DIR}" "M=${CMAKE_SOURCE_DIR}" clean
  COMMAND ${CMAKE_COMMAND} -E rm -rf "${MODULE_OUTPUT_DIR}" build_kernel.log
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMENT "Cleaning kernel module build artifacts (kclean)"
  VERBATIM
)

add_custom_target(kinstall
  DEPENDS kbuild
  COMMAND ${CMAKE_COMMAND} -E make_directory "${MODULE_OUTPUT_DIR}"
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
          "${CMAKE_SOURCE_DIR}/${MODULE_NAME}.ko"
          "${MODULE_OUTPUT_DIR}/${MODULE_NAME}.ko"
  COMMENT "Copying ${MODULE_NAME}.ko to ${MODULE_OUTPUT_DIR} (kinstall)"
  VERBATIM
)

add_custom_target(show
  COMMAND ${CMAKE_COMMAND} -E echo "Kernel dir: ${KERNEL_DIR}"
  COMMAND ${CMAKE_COMMAND} -E echo "Module source: ${MODULE_SRC}"
  COMMAND ${CMAKE_COMMAND} -E echo "Built module (after kinstall): ${MODULE_OUTPUT_DIR}/${MODULE_NAME}.ko"
  VERBATIM
)

message(STATUS "CMake configured for kernel module build")
message(STATUS "Detected KERNEL_DIR=${KERNEL_DIR}")
message(STATUS "Run: cmake . && make")
message(STATUS "If the build fails, inspect build_kernel.log and follow the printed diagnostic checklist.")
