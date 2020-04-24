#ifndef SCENEWRITER_HPP_
#define SCENEWRITER_HPP_

#include "AssetReader.hpp"
#include "ObjectModule.hpp"
#include "SceneModule.hpp"

class SceneWriter
{
public:
    SceneWriter() = default;
    ~SceneWriter() = default;

    void tempScene();
    void saveScene();

protected:
private:
    AssetReader ar;
    ObjectModule om;
    SceneModule sm;
};

#endif /* !SCENEWRITER_HPP_ */
