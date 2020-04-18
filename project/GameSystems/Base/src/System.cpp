#include "System.hpp"

#include "Entity.hpp"

void System::process(Entity* entity, UpdateType updateType)
{
    if( assertEntity(entity) )
    {
        switch(updateType)
        {
            case START:
                return start();

            case FIXED:
                return fixedUpdate();

            case FRAME:
                return frameUpdate();
        }

    }
}