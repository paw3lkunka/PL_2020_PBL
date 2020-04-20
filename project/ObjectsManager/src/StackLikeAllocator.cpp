#include <cstdlib>

#include "StackLikeAllocator.hpp"

StackLikeAllocator::StackLikeAllocator(int size)
{   
    this->size = size;
    buffer = (char*)malloc(size);
    top = buffer;
}

StackLikeAllocator::~StackLikeAllocator()
{
    free(buffer);
}

void StackLikeAllocator::clear()
{
    top = buffer;
}

int StackLikeAllocator::getSize()
{
    return size;
}

int StackLikeAllocator::getFreeSpace()
{
    return size - (top - buffer);
}

StackLikeAllocatorOverflowException::StackLikeAllocatorOverflowException(int objSize, int freeSpace)
{
    this->freeSpace = freeSpace;
    this->objectSize = objectSize;
}

const char* StackLikeAllocatorOverflowException::what() const noexcept
{
    return "It is to few free space to fit object\n";
}