#ifndef _TERRAINUTILS_HPP
#define _TERRAINUTILS_HPP

#include <glm/glm.hpp>
#include <string>

namespace TerrainUtils
{
    /**
     * @brief Loads all models from Terrain folder
     */
    void loadAllTerrainChunks();
    /**
     * @brief Loads all models from Models/Unity
     */
    void loadExportedUnityModels();
    /**
     * @brief Creates materials For Models object
     */
    void createMaterialsForModels();
    /**
     * @brief Converts space-separated string to vec3
     * 
     * @return glm::vec3 parsed vector
     */
    glm::vec3 stringToVec3(std::string line);
    /**
     * @brief Converts 
     * 
     * @param line 
     * @return glm::quat 
     */
    glm::quat stringToQuat(std::string line);
}

#endif // _TERRAINUTILS_HPP
