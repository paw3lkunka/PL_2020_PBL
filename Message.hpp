#ifndef MESSANGE_HPP_
#define MESSANGE_HPP_

#include <exception>
#include "Event.hpp"

constexpr int BUFFER_SIZE = 32 - sizeof(Event);

template<typename T>
class MessageBufferOverflowException : public std::exception
{
    public:
        MessageBufferOverflowException(T value)
        {
            this->value = value;
        }
        T value;
        virtual const char* what() const noexcept
        {
            return "Size of value is too big to fit in Message buffer\n";
        } 
};

class Message 
{
    public:
    
        Message(Event event)
        {            
            this->event = event;
        }

        template<typename T>
        Message(Event event, T value = nullptr)
        {
            this->event = event;

            if( sizeof(value) > BUFFER_SIZE )
            {
                throw MessageBufferOverflowException<T>(value);
            }

            T* ptr = reinterpret_cast<T*>(buffer);
            *ptr = value;
        }
        Event getEvent()
        {
            return event;
        }
        template<typename T>
        T getValue()
        {
            T* ptr = reinterpret_cast<T*>(buffer);
            return *ptr;
        }
    private:
        Event event;
        char buffer[BUFFER_SIZE];
};

#endif /* !MESSAGE_HPP_ */
