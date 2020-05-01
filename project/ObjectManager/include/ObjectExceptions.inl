#ifndef OBJECTEXCEPTIONS_INL_
#define OBJECTEXCEPTIONS_INL_
#include <exception>

struct TooManyInstancesException : public std::exception
{
    std::string message;

    TooManyInstancesException(const char* type)
    {
        message = "Too many instances of object ";
        message += type;
    }

    const char* what() const noexcept
    {
        return message.c_str();
    }
};

struct FileNotFoundException : public std::exception
{
    std::string message;

    FileNotFoundException(const char* type)
    {
        message = "File not found ";
        message += type;
    }

    const char* what() const noexcept
    {
        return message.c_str();
    }
};

struct AssetLoadingException : public std::exception
{
    std::string message;

    AssetLoadingException(const char* type)
    {
        message = "Error while loading asset ";
        message += type;
    }

    const char* what() const noexcept
    {
        return message.c_str();
    }
};

#endif /* !OBJECTEXCEPTIONS_INL_ */
