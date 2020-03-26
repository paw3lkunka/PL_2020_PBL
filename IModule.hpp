/*
** EPITECH PROJECT, 2020
** test
** File description:
** IModule
*/

#ifndef IMODULE_HPP_
#define IMODULE_HPP_

#include "Message.hpp"

class IModule {
    public:
        virtual void ReceiveMessage(Message msg) = 0;
};

#endif /* !IMODULE_HPP_ */
