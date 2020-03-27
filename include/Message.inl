#ifndef MESSANGE_HPP_
#define MESSANGE_HPP_

#include <exception>
#include "Event.hpp"

/**
 * @brief Thrown by Message constructor when additional data are to big, to fit in buffer 
 * 
 * @tparam T type of additional data
 */
template<typename T>
class MessageBufferOverflowException : public std::exception
{
    public:
        /**
         * @brief Construct a new Message Buffer Overflow Exception object
         * 
         * @param value data, which wasn't fit into buffer
         */
        MessageBufferOverflowException(T value)
        {
            this->value = value;
        }
        /**
         * @brief Data, which wasn't fit into buffer         * 
         */
        T value;

        /**
         * @brief Returns error log
         * 
         * @return const char* error log
         */
        virtual const char* what() const noexcept
        {
            return "Size of value is too big to fit in Message buffer\n";
        } 
};

/**
 * @brief Message used to communicate via service buss,
 * can carry additional data every type, as long, as they fits in buffer 
 */
class Message 
{
    public:
        /**
         * @brief Determine size of additional data buffer
         */
        static constexpr int BUFFER_SIZE = 32 - sizeof(Event);
    
        /**
         * @brief Construct a new Message object
         * 
         * @param event - Type of messange
         */
        inline Message(Event event = Event::UNKNOWN_EVENT)
        {            
            this->event = event;
        }

        /**
         * @brief Construct a new Message object
         * 
         * @tparam T - Type of additional data
         * @param event - Type of messange
         * @param value - Additional data
         */
        template<typename T>
        Message(Event event, T value)
        {
            this->event = event;

            if( sizeof(value) > BUFFER_SIZE )
            {
                throw MessageBufferOverflowException<T>(value);
            }

            T* ptr = reinterpret_cast<T*>(buffer);
            *ptr = value;
        }

        /**
         * @brief Get the Event object
         * 
         * @return Event - Type of messange
         */
        inline Event getEvent()
        {
            return event;
        }

        /**
         * @brief Get the Value object
         * 
         * @tparam T - Type of data to extract
         * @return T - Stored additional data
         */
        template<typename T>
        T getValue()
        {
            T* ptr = reinterpret_cast<T*>(buffer);
            return *ptr;
        }
    private:
        /**
         * @brief Representation of Message type 
         */
        Event event;

        /**
         * @brief Store additional data 
         */
        char buffer[BUFFER_SIZE];
};

#endif /* !MESSAGE_HPP_ */
