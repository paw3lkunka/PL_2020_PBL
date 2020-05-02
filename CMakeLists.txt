cmake_minimum_required( VERSION 3.4 )

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

execute_process(COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/components.sh)
execute_process(COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/systems.sh)

# Eventually clone submodules
if(NOT EXISTS "libs/assimp/CMakeLists.txt")
    execute_process(COMMAND git submodule update --init "libs/assimp/" WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif()

if(NOT EXISTS "libs/glfw/CMakeLists.txt")
    execute_process(COMMAND git submodule update --init "libs/glfw/" WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif()

if(NOT EXISTS "libs/glm/CMakeLists.txt")
    execute_process(COMMAND git submodule update --init "libs/glm/" WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif()

if(NOT EXISTS "libs/stb/README.md")
    execute_process(COMMAND git submodule update --init "libs/stb/" WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif()

if(NOT EXISTS "libs/openal-soft/CMakeLists.txt")
    execute_process(COMMAND git submodule update --init "libs/openal-soft/" WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif()

project( PBL )
enable_language( CXX )

# Enable GLFW unused features 
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# Enable GLFW unused eatures 
set(ALSOFT_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ALSOFT_TESTS OFF CACHE BOOL "" FORCE)

# Enable Assimp modules
set(ASSIMP_NO_EXPORT OFF CACHE BOOL "" FORCE)

set(ASSIMP_BUILD_FBX_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_GLTF_IMPORTER ON CACHE BOOL "" FORCE)

set(ASSIMP_BUILD_GLTF_EXPORTER ON CACHE BOOL "" FORCE)

# Disable Assimp modules
set(ASSIMP_BUILD_AMF_IMPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_3DS_IMPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_AC_IMPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASE_IMPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSBIN_IMPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_B3D_IMPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_BVH_IMPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_COLLADA_IMPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_DXF_IMPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_CSM_IMPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_HMP_IMPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_IRRMESH_IMPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_IRR_IMPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_LWO_IMPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_LWS_IMPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_MD2_IMPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_MD3_IMPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_MD5_IMPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_MDC_IMPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_MDL_IMPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_NFF_IMPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_NDO_IMPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_OFF_IMPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_OBJ_IMPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_OGRE_IMPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_OPENGEX_IMPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_PLY_IMPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_MS3D_IMPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_COB_IMPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_BLEND_IMPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_IFC_IMPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_XGL_IMPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_Q3D_IMPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_Q3BSP_IMPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_RAW_IMPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_SIB_IMPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_SMD_IMPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_STL_IMPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TERRAGEN_IMPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_3D_IMPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_X_IMPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_X3D_IMPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_3MF_IMPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_MMD_IMPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_STEP_IMPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_M3D_IMPORTER OFF CACHE BOOL "" FORCE)

set(ASSIMP_BUILD_3DS_EXPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSBIN_EXPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSXML_EXPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_COLLADA_EXPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_OBJ_EXPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_OPENGEX_EXPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_PLY_EXPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_FBX_EXPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_STL_EXPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_X_EXPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_X3D_EXPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_3MF_EXPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSJSON_EXPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_STEP_EXPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_M3D_EXPORTER OFF CACHE BOOL "" FORCE)

set(BUILD_DOCS OFF CACHE BOOL "" FORCE) #Assimp
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)

add_subdirectory(libs/glad/)
add_subdirectory(libs/glfw/)
add_subdirectory(libs/glm/)
add_subdirectory(libs/openal-soft)
add_subdirectory(libs/assimp/)

aux_source_directory( project                               PROJECT_SRC)
aux_source_directory( project/Core/src                      CORE_SRC)
aux_source_directory( project/GameSystems/Base/src          GAME_SRC)
aux_source_directory( project/GameSystems/Systems/src       SYSTEMS_SRC)
aux_source_directory( project/Audio/src                     AUDIO_SRC)
aux_source_directory( project/ResourceManager/src           FILE_SYSTEM_SRC)
aux_source_directory( project/ObjectsManager/src            OBJECTS_SRC)
aux_source_directory( project/Input/src                     INPUT_SRC)
aux_source_directory( project/MessageBus/src                MESSAGE_BUS_SRC)
aux_source_directory( project/Renderer/src                  RENDERER_SRC)
aux_source_directory( project/Terminal/src                  TERMIANL_SRC)
aux_source_directory( project/Scene/src                     SCENE_SRC)

list( 
    APPEND SOURCE_FILES
        ${CORE_SRC}
        ${GAME_SRC}
        ${SYSTEMS_SRC}
        ${PROJECT_SRC}
        ${AUDIO_SRC}
        ${FILE_SYSTEM_SRC}
        ${OBJECTS_SRC}
        ${INPUT_SRC}
        ${MESSAGE_BUS_SRC}
        ${RENDERER_SRC}
        ${TERMIANL_SRC}
        ${SCENE_SRC}
)

add_library(lib ${SOURCE_FILES} )

include_directories( 
    project
    project/Autogen/include
    project/Core/include
    project/GameSystems/Base/include
    project/GameSystems/Components
    project/GameSystems/Systems/include
    project/Audio/include
    project/ResourceManager/include
    project/ObjectsManager/include
    project/Input/include
    project/MessageBus/include
    project/Renderer/include
    project/Terminal/include
    project/Scene/include
    libs/stb
    libs/glad/include
    libs/glfw/include
    libs/assimp/include
    libs/glm/
    libs/openal-soft/include
    build/libs/assimp/include
)

add_executable( PBL project/main.cpp )

target_link_libraries( 
    PBL
        lib
        glad
        glfw
        glm
        OpenAL
        assimp
)    

file(GLOB resources "resources/*")
file(COPY ${resources} DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/Resources")
