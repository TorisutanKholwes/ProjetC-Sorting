cmake_minimum_required(VERSION 3.22)
project(SDLBase C)
set(CMAKE_C_STANDARD 23)

set(LIB_INSTALL_DIR "${CMAKE_SOURCE_DIR}/libs")

if (WIN32)
    set(CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/libs/cmake")
else ()
    set(CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/libs/lib/cmake/SDL3" "${CMAKE_SOURCE_DIR}/libs/lib/cmake/SDL3_image" "${CMAKE_SOURCE_DIR}/libs/lib/cmake/SDL3_mixer" "${CMAKE_SOURCE_DIR}/libs/lib/cmake/SDL3_ttf")
endif ()

find_package(SDL3 QUIET)
find_package(SDL3_image QUIET)
find_package(SDL3_mixer QUIET)
find_package(SDL3_ttf QUIET)

if(NOT SDL3_FOUND OR NOT SDL3_image_FOUND OR NOT SDL3_mixer_FOUND OR NOT SDL3_ttf_FOUND)
    message(WARNING "SDL3 or one of its extensions not found. Attempting local installation in ${LIB_INSTALL_DIR}...")

    if (WIN32)
        message(STATUS "Installing SDL3 and extensions on Windows using PowerShell script.")
        set(INSTALL_SCRIPT "${CMAKE_SOURCE_DIR}/install_sdl3.ps1")
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
    find_package(SDL3 REQUIRED)
    find_package(SDL3_image REQUIRED)
    find_package(SDL3_mixer REQUIRED)
    find_package(SDL3_ttf REQUIRED)

    if (SDL3_FOUND AND SDL3_image_FOUND AND SDL3_mixer_FOUND AND SDL3_ttf_FOUND)
        message(STATUS "SDL3 and all extensions successfully installed and found. If your encounter issues with includes, try clearing caches of your IDE and restart project.")
    else ()
        message(FATAL_ERROR "Failed to find SDL3 or one of its extensions after installation.")
    endif ()
else ()
    message(STATUS "SDL3 and all extensions already installed.")
endif ()

file(GLOB SRC_FILES "src/*.c" "src/**/*.c")

message(STATUS "Source files: ${SRC_FILES}")

add_executable(SDLBase ${SRC_FILES} )

target_include_directories(SDLBase PRIVATE include)
target_include_directories(SDLBase PRIVATE ${SDL3_INCLUDE_DIRS} ${SDL3_IMAGE_INCLUDE_DIRS} ${SDL3_MIXER_INCLUDE_DIRS} ${SDL3_TTF_INCLUDE_DIRS})

target_link_libraries(SDLBase PRIVATE SDL3::SDL3 SDL3_image::SDL3_image SDL3_mixer::SDL3_mixer SDL3_ttf::SDL3_ttf m)
if (NOT WIN32)
    target_compile_options(SDLBase PRIVATE -Wall -Wextra)
endif ()
