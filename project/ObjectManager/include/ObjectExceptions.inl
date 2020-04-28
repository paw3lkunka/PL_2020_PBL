#ifndef OBJECTEXCEPTIONS_INL_
#define OBJECTEXCEPTIONS_INL_
#include <exception>

struct TooManyInstancesException : public std::exception
{
    const char* message;

    TooManyInstancesException(const char* type)
    {
        message = "Too many instances of object %s", type;
    }

    const char* what()
    {
        return message;
    }
};

struct AssetLoadingException : public std::exception
{
    const char* message;

    AssetLoadingException(const char* type)
    {
        message = "Error while loading asset %s", type;
    }

    const char* what()
    {
        return message;
    }
};

#endif /* !OBJECTEXCEPTIONS_INL_ */
