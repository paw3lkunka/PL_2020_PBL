#include "System.hpp"

#include "Entity.hpp"

void System::process(Entity* entity, UpdateType updateType)
{
    if( assertEntity(entity) )
    {
        switch(updateType)
        {
            case FIXED:
                return fixedUpdate();

            case FRAME:
                return frameUpdate();
        }

    }
}