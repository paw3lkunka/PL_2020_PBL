#include "SceneWriter.hpp"

#include <json.hpp>
#include <fstream>
#include <iomanip>


void SceneWriter::saveScene()
{
    nlohmann::json j;
    std::string name;
    std::vector<unsigned int> childrenID;
    for( int i = 0; i < om.entities.size(); ++i)
    {
        childrenID.clear();
        name = "entity" + std::to_string(i);
        j[name]["id"] = om.entities[i].getId();
        j[name]["serialization ID"] = om.entities[i].serializationID;
        std::cout << om.entities[i].getComponentsPtr()->size() << "\t";
        for(int j = 0; j < om.entities[i].getComponentsPtr()->size(); ++j)
        {
            childrenID.push_back(om.entities[i].getComponentsPtr()->operator[](j)->serializationID);
        }
        j[name]["components"] = childrenID;
    }

    for( int i = 0; i < om.components.size(); ++i)
    {
        name = "component" + std::to_string(i);
        j[name]["entity id"] = om.components[i]->entityPtr->getId();
        j[name]["serialization ID"] = om.components[i]->serializationID;
        if(dynamic_cast<Transform*>(om.components[i]))
        {
            Transform* temp = dynamic_cast<Transform*>(om.components[i]);
            j[name]["type"] = "Transform";
            if(temp->getParent() != nullptr)
                j[name]["transform parentID"] = temp->getParent()->serializationID;
            j[name]["localPosition"]["x"] = temp->getLocalPosition().x;
            j[name]["localPosition"]["y"] = temp->getLocalPosition().y;
            j[name]["localPosition"]["z"] = temp->getLocalPosition().z;
            j[name]["localRotation"]["x"] = temp->getLocalRotation().x;
            j[name]["localRotation"]["y"] = temp->getLocalRotation().y;
            j[name]["localRotation"]["z"] = temp->getLocalRotation().z;
            j[name]["localRotation"]["w"] = temp->getLocalRotation().w;
            j[name]["localScale"]["x"] = temp->getLocalScale().x;
            j[name]["localScale"]["y"] = temp->getLocalScale().y;
            j[name]["localScale"]["z"] = temp->getLocalScale().z;
        }
        if(dynamic_cast<Bone*>(om.components[i]))
        {
            Bone* temp = dynamic_cast<Bone*>(om.components[i]);
            j[name]["type"] = "Bone";
            j[name]["filePath"] = temp->filePath;
        }
    }

    std::ofstream file("test.json");
    if(file.good())
    {
        file << std::setw(4) << j;
    }
    file.close();
}