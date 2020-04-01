cmake_minimum_required( VERSION 3.4 )

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project( PBL )
enable_language( CXX )

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

set(BUILD_DOCS OFF CACHE BOOL "" FORCE) #Assimp
set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)

add_subdirectory(libs/glad/)
add_subdirectory(libs/glfw-3.3.2/)
add_subdirectory(libs/glm/)
add_subdirectory(libs/assimp-5.0.1/)

aux_source_directory( project                   PROJECT_SRC)
aux_source_directory( project/GameSystems/src   GAME_SRC)
aux_source_directory( project/Audio/src         AUDIO_SRC)
aux_source_directory( project/FileSystem/src    FILE_SYSTEM_SRC)
aux_source_directory( project/Input/src         INPUT_SRC)
aux_source_directory( project/MessageBus/src    MESSAGE_BUS_SRC)
aux_source_directory( project/Renderer/src      RENDERER_SRC)
aux_source_directory( project/Terminal/src      TERMIANL_SRC)

list( 
    APPEND SOURCE_FILES
        ${GAME_SRC}
        ${PROJECT_SRC}
        ${AUDIO_SRC}
        ${FILE_SYSTEM_SRC}
        ${INPUT_SRC}
        ${MESSAGE_BUS_SRC}
        ${RENDERER_SRC}
        ${TERMIANL_SRC}
)

add_library(lib ${SOURCE_FILES} )

include_directories( 
    project
    project/GameSystems/include
    project/Audio/include
    project/FileSystem/include
    project/Input/include
    project/MessageBus/include
    project/Renderer/include
    project/Terminal/include
    libs/stb
)

add_executable( PBL project/main.cpp )

target_link_libraries( 
    PBL
        lib
        glad
        glfw
        glm
        assimp
) 


