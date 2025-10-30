cmake_minimum_required(VERSION 3.22)
project(ProjectC-Sorting C)
set(CMAKE_C_STANDARD 23)

set(LIB_INSTALL_DIR "${CMAKE_SOURCE_DIR}/libs")

if (WIN32)
    set(CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/libs/cmake")
else ()
    set(CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/libs/lib/cmake/SDL2" "${CMAKE_SOURCE_DIR}/libs/lib/cmake/SDL2_image" "${CMAKE_SOURCE_DIR}/libs/lib/cmake/SDL2_mixer" "${CMAKE_SOURCE_DIR}/libs/lib/cmake/SDL2_ttf")
endif ()

find_package(SDL2 QUIET)
find_package(SDL2_image QUIET)
find_package(SDL2_mixer QUIET)
find_package(SDL2_ttf QUIET)

if(NOT SDL2_FOUND OR NOT SDL2_image_FOUND OR NOT SDL2_mixer_FOUND OR NOT SDL2_ttf_FOUND)
    message(WARNING "SDL3 or one of its extensions not found. Attempting local installation in ${LIB_INSTALL_DIR}...")

    if (WIN32)
        message(STATUS "Installing SDL3 and extensions on Windows using PowerShell script.")
        set(INSTALL_SCRIPT "${CMAKE_SOURCE_DIR}/install_libs.ps1")
        if(NOT EXISTS "${INSTALL_SCRIPT}")
            message(FATAL_ERROR "Missing PowerShell install script: ${INSTALL_SCRIPT}")
        endif()

        execute_process(
                COMMAND powershell -ExecutionPolicy Bypass -File "${INSTALL_SCRIPT}" -InstallDir "${LIB_INSTALL_DIR}"
                WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
                RESULT_VARIABLE ps_result
        )

        if(NOT ps_result EQUAL 0)
            message(FATAL_ERROR "PowerShell SDL3 installation script failed (exit code ${ps_result}).")
        endif()
    elseif (UNIX)
        message(STATUS "Installing SDL3 and extensions on Unix using Bash script.")
        set(INSTALL_SCRIPT "${CMAKE_SOURCE_DIR}/install_sdl3.sh")
        if(NOT EXISTS "${INSTALL_SCRIPT}")
            message(FATAL_ERROR "Missing Bash install script: ${INSTALL_SCRIPT}")
        endif()

        execute_process(
                COMMAND bash "${INSTALL_SCRIPT}" "${LIB_INSTALL_DIR}"
                WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
                RESULT_VARIABLE sh_result
        )

        if(NOT sh_result EQUAL 0)
            message(FATAL_ERROR "Bash SDL3 installation script failed (exit code ${sh_result}).")
        endif()
    endif ()

    message(STATUS "Re-attempting to find SDL3 and extensions after installation...")

    list(APPEND CMAKE_PREFIX_PATH "${LIB_INSTALL_DIR}")
    find_package(SDL2 REQUIRED)
    find_package(SDL2_image REQUIRED)
    find_package(SDL2_mixer REQUIRED)
    find_package(SDL2_ttf REQUIRED)

    if (SDL3_FOUND AND SDL3_image_FOUND AND SDL3_mixer_FOUND AND SDL3_ttf_FOUND)
        message(STATUS "SDL3 and all extensions successfully installed and found. If your encounter issues with includes, try clearing caches of your IDE and restart project.")
    else ()
        message(FATAL_ERROR "Failed to find SDL3 or one of its extensions after installation.")
    endif ()
else ()
    message(STATUS "SDL3 and all extensions already installed.")
endif ()

set(TINFYD_DIR "${CMAKE_SOURCE_DIR}/libs/tinyfiledialogs")

add_library(tinyfiledialogs STATIC "${TINFYD_DIR}/tinyfiledialogs.c")
target_include_directories(tinyfiledialogs PUBLIC "${TINFYD_DIR}")

if (APPLE)
    target_link_libraries(tinyfiledialogs PRIVATE "-framework Cocoa" "-framework Carbon")
endif()

file(GLOB SRC_FILES "src/*.c" "src/**/*.c")

message(STATUS "Source files: ${SRC_FILES}")

add_executable(ProjectC-Sorting ${SRC_FILES} )

target_include_directories(ProjectC-Sorting PRIVATE include)
target_include_directories(ProjectC-Sorting PRIVATE ${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS} ${SDL2_MIXER_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS})

if (WIN32)
    target_link_libraries(ProjectC-Sorting PRIVATE SDL2::SDL2 SDL2::SDL2main SDL2_image::SDL2_image SDL2_mixer::SDL2_mixer SDL2_ttf::SDL2_ttf tinyfiledialogs)
else ()
    target_link_libraries(ProjectC-Sorting PRIVATE SDL2::SDL2 SDL2_image::SDL2_image SDL2_mixer::SDL2_mixer SDL2_ttf::SDL2_ttf tinyfiledialogs m)
endif ()

if (NOT WIN32)
    target_compile_options(ProjectC-Sorting PRIVATE -Wall -Wextra -Werror)
endif ()
