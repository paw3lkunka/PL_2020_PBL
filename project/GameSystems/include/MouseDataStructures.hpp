#ifndef MOUSEDATASTRUCTURES_HPP_
#define MOUSEDATASTRUCTURES_HPP_

#define VERTICAL_SCROLL 0
#define HORIZONTAL_SCROLL 1

/**
 * @brief Structure carring mouse cursor data
 */
struct CursorData
{
    /// @brief horizontal coordinate of cursor
    float xPos;
    /// @brief horizontal coordinate of cursor
    float yPos;
    /// @brief chaange of horizontal coordinate of cursor
    float xDelta;
    /// @brief chaange of horizontal coordinate of cursor
    float yDelta;
};

/**
 * @brief Structure carring mouse scroll data
 */
struct ScrollData
{    
    /// @brief offset of scroll
    double offset;
    /// @brief VERTICAL_SCROLL || HORIZONTAL_SCROLL
    int axis;
};


#endif /* !MOUSEDATASTRUCTURES_HPP_ */
