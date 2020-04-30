#ifndef SCENEREADER_HPP_
#define SCENEREADER_HPP_

#include <json.hpp>

class AssetReader;
class ObjectModule;


class SceneReader
{
public:
    SceneReader() = default;
    ~SceneReader() = default;

protected:
private:
    ObjectModule* objModulePtr;
    AssetReader* assReaderPtr;

    nlohmann::json j;
};

#endif /* !SCENEREADER_HPP_ */
