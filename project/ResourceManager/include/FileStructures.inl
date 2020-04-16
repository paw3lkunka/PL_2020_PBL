#ifndef FILESTRUCTUERS_INL_
#define FILESTRUCTUERS_INL_

/**
 * @brief Enum class that contains types of files to process by resource module
 */
enum class FileType : unsigned int
{
    AUDIO,
    SHADER,
    MESH_WITH_TEXTURES,
    MESH,
    TEXTURE,
};

/**
 * @brief Basic file data in Filesystem
 */
struct FileSystemData
{
    const char* path;
    FileType typeOfFile;
};

#endif /* !FILESTRUCTUERS_INL_ */
