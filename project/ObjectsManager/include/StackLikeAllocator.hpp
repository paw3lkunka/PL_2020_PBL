#ifndef STACKLIKEALLOCATOR_HPP_
#define STACKLIKEALLOCATOR_HPP_

#include <exception>

class StackLikeAllocatorOverflowException : public std::exception
{
    public:
        StackLikeAllocatorOverflowException(int objSize, int freeSpace);
        virtual const char* what() const noexcept;
        int freeSpace;
        int objectSize;
    
};

/**
 * @brief fixed size buffer allowing to continuously allocate objects of any type.
 * Allocated object can be removed only by clearing whole buffer.
 */
class StackLikeAllocator
{
    public:
        /**
         * @brief Construct a new Stack Like Allocator object. 
         * @param size size of buffer in bytes.
         */
        StackLikeAllocator(int size);
        /**
         * @brief Destroy the Stack Like Allocator object
         * free reserved space. 
         */
        virtual ~StackLikeAllocator();

        /**
         * @brief Allocate new object, and return pointer to it.
         * 
         * @tparam T - Type of object.
         * @param obj - Object to allocate.
         * @return T* - Pointer to allocated object
         */
        template<typename T>
        T* allocate(T obj);

        /**
         * @brief Whip all stored data.  
         */
        void clear();

        /**
         * @brief Get size of reserved space in bytes.
         * 
         * @return int 
         */
        int getSize();

        /**
         * @brief Get free space in bytes.
         * 
         * @return int 
         */
        int getFreeSpace();

    protected:
    private:
        /// @brief size of reserved space in bytes.
        int size;
        
        /// @brief size of reserved space in bytes.
        char* buffer;
        
        /// @brief size of reserved space in bytes.
        char* top;
};

#include "StackLikeAllocator.ipp"

#endif /* !STACKLIKEALLOCATOR_HPP_ */
