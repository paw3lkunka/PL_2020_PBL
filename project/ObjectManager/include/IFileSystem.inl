#ifndef IFILESYSTEM_INL_
#define IFILESYSTEM_INL_

#include <string>

class SceneWriter;

class IFileSystem
{
public:
    IFileSystem() = default;
    IFileSystem(std::string filePath) : filePath(filePath) {}
    virtual ~IFileSystem() = default;
private:
    std::string filePath;
    friend class SceneWriter;
};

#endif /* !IFILESYSTEM_INL_ */
