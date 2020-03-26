#include "Message.hpp"
#include <iostream>
#include <utility>

typedef std::pair<float,float> float2;
int main()
{
    std::cout << "Henlo!" << std::endl;
    Message m1(Event::KEY_PRESSED_A);
    Message m2(Event::KEY_PRESSED_A, 123);
    Message m3(Event::KEY_PRESSED_A, 0.2);
    Message m4(Event::KEY_PRESSED_A, float2(21.0f,37.0f));
    std::cout << m2.getValue<int>() << ", " << m3.getValue<double>() << std::endl;
    std::cout << m4.getValue<float2>().first << ", " << m4.getValue<float2>().second << std::endl;
    return 0;
}